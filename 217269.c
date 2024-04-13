add_card(struct pci_dev *dev, const struct pci_device_id *unused)
{
	struct pcilynx *lynx;
	u32 p, end;
	int ret, i;

	if (pci_set_dma_mask(dev, DMA_BIT_MASK(32))) {
		dev_err(&dev->dev,
		    "DMA address limits not supported for PCILynx hardware\n");
		return -ENXIO;
	}
	if (pci_enable_device(dev)) {
		dev_err(&dev->dev, "Failed to enable PCILynx hardware\n");
		return -ENXIO;
	}
	pci_set_master(dev);

	lynx = kzalloc(sizeof *lynx, GFP_KERNEL);
	if (lynx == NULL) {
		dev_err(&dev->dev, "Failed to allocate control structure\n");
		ret = -ENOMEM;
		goto fail_disable;
	}
	lynx->pci_device = dev;
	pci_set_drvdata(dev, lynx);

	spin_lock_init(&lynx->client_list_lock);
	INIT_LIST_HEAD(&lynx->client_list);
	kref_init(&lynx->kref);

	lynx->registers = ioremap(pci_resource_start(dev, 0),
					  PCILYNX_MAX_REGISTER);
	if (lynx->registers == NULL) {
		dev_err(&dev->dev, "Failed to map registers\n");
		ret = -ENOMEM;
		goto fail_deallocate_lynx;
	}

	lynx->rcv_start_pcl = pci_alloc_consistent(lynx->pci_device,
				sizeof(struct pcl), &lynx->rcv_start_pcl_bus);
	lynx->rcv_pcl = pci_alloc_consistent(lynx->pci_device,
				sizeof(struct pcl), &lynx->rcv_pcl_bus);
	lynx->rcv_buffer = pci_alloc_consistent(lynx->pci_device,
				RCV_BUFFER_SIZE, &lynx->rcv_buffer_bus);
	if (lynx->rcv_start_pcl == NULL ||
	    lynx->rcv_pcl == NULL ||
	    lynx->rcv_buffer == NULL) {
		dev_err(&dev->dev, "Failed to allocate receive buffer\n");
		ret = -ENOMEM;
		goto fail_deallocate_buffers;
	}
	lynx->rcv_start_pcl->next	= cpu_to_le32(lynx->rcv_pcl_bus);
	lynx->rcv_pcl->next		= cpu_to_le32(PCL_NEXT_INVALID);
	lynx->rcv_pcl->async_error_next	= cpu_to_le32(PCL_NEXT_INVALID);

	lynx->rcv_pcl->buffer[0].control =
			cpu_to_le32(PCL_CMD_RCV | PCL_BIGENDIAN | 2044);
	lynx->rcv_pcl->buffer[0].pointer =
			cpu_to_le32(lynx->rcv_buffer_bus + 4);
	p = lynx->rcv_buffer_bus + 2048;
	end = lynx->rcv_buffer_bus + RCV_BUFFER_SIZE;
	for (i = 1; p < end; i++, p += 2048) {
		lynx->rcv_pcl->buffer[i].control =
			cpu_to_le32(PCL_CMD_RCV | PCL_BIGENDIAN | 2048);
		lynx->rcv_pcl->buffer[i].pointer = cpu_to_le32(p);
	}
	lynx->rcv_pcl->buffer[i - 1].control |= cpu_to_le32(PCL_LAST_BUFF);

	reg_set_bits(lynx, MISC_CONTROL, MISC_CONTROL_SWRESET);
	/* Fix buggy cards with autoboot pin not tied low: */
	reg_write(lynx, DMA0_CHAN_CTRL, 0);
	reg_write(lynx, DMA_GLOBAL_REGISTER, 0x00 << 24);

#if 0
	/* now, looking for PHY register set */
	if ((get_phy_reg(lynx, 2) & 0xe0) == 0xe0) {
		lynx->phyic.reg_1394a = 1;
		PRINT(KERN_INFO, lynx->id,
		      "found 1394a conform PHY (using extended register set)");
		lynx->phyic.vendor = get_phy_vendorid(lynx);
		lynx->phyic.product = get_phy_productid(lynx);
	} else {
		lynx->phyic.reg_1394a = 0;
		PRINT(KERN_INFO, lynx->id, "found old 1394 PHY");
	}
#endif

	/* Setup the general receive FIFO max size. */
	reg_write(lynx, FIFO_SIZES, 255);

	reg_set_bits(lynx, PCI_INT_ENABLE, PCI_INT_DMA_ALL);

	reg_write(lynx, LINK_INT_ENABLE,
		  LINK_INT_PHY_TIME_OUT | LINK_INT_PHY_REG_RCVD |
		  LINK_INT_PHY_BUSRESET | LINK_INT_IT_STUCK |
		  LINK_INT_AT_STUCK | LINK_INT_SNTRJ |
		  LINK_INT_TC_ERR | LINK_INT_GRF_OVER_FLOW |
		  LINK_INT_ITF_UNDER_FLOW | LINK_INT_ATF_UNDER_FLOW);

	/* Disable the L flag in self ID packets. */
	set_phy_reg(lynx, 4, 0);

	/* Put this baby into snoop mode */
	reg_set_bits(lynx, LINK_CONTROL, LINK_CONTROL_SNOOP_ENABLE);

	run_pcl(lynx, lynx->rcv_start_pcl_bus, 0);

	if (request_irq(dev->irq, irq_handler, IRQF_SHARED,
			driver_name, lynx)) {
		dev_err(&dev->dev,
			"Failed to allocate shared interrupt %d\n", dev->irq);
		ret = -EIO;
		goto fail_deallocate_buffers;
	}

	lynx->misc.parent = &dev->dev;
	lynx->misc.minor = MISC_DYNAMIC_MINOR;
	lynx->misc.name = "nosy";
	lynx->misc.fops = &nosy_ops;

	mutex_lock(&card_mutex);
	ret = misc_register(&lynx->misc);
	if (ret) {
		dev_err(&dev->dev, "Failed to register misc char device\n");
		mutex_unlock(&card_mutex);
		goto fail_free_irq;
	}
	list_add_tail(&lynx->link, &card_list);
	mutex_unlock(&card_mutex);

	dev_info(&dev->dev,
		 "Initialized PCILynx IEEE1394 card, irq=%d\n", dev->irq);

	return 0;

fail_free_irq:
	reg_write(lynx, PCI_INT_ENABLE, 0);
	free_irq(lynx->pci_device->irq, lynx);

fail_deallocate_buffers:
	if (lynx->rcv_start_pcl)
		pci_free_consistent(lynx->pci_device, sizeof(struct pcl),
				lynx->rcv_start_pcl, lynx->rcv_start_pcl_bus);
	if (lynx->rcv_pcl)
		pci_free_consistent(lynx->pci_device, sizeof(struct pcl),
				lynx->rcv_pcl, lynx->rcv_pcl_bus);
	if (lynx->rcv_buffer)
		pci_free_consistent(lynx->pci_device, PAGE_SIZE,
				lynx->rcv_buffer, lynx->rcv_buffer_bus);
	iounmap(lynx->registers);

fail_deallocate_lynx:
	kfree(lynx);

fail_disable:
	pci_disable_device(dev);

	return ret;
}