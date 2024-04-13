irq_handler(int irq, void *device)
{
	struct pcilynx *lynx = device;
	u32 pci_int_status;

	pci_int_status = reg_read(lynx, PCI_INT_STATUS);

	if (pci_int_status == ~0)
		/* Card was ejected. */
		return IRQ_NONE;

	if ((pci_int_status & PCI_INT_INT_PEND) == 0)
		/* Not our interrupt, bail out quickly. */
		return IRQ_NONE;

	if ((pci_int_status & PCI_INT_P1394_INT) != 0) {
		u32 link_int_status;

		link_int_status = reg_read(lynx, LINK_INT_STATUS);
		reg_write(lynx, LINK_INT_STATUS, link_int_status);

		if ((link_int_status & LINK_INT_PHY_BUSRESET) > 0)
			bus_reset_irq_handler(lynx);
	}

	/* Clear the PCI_INT_STATUS register only after clearing the
	 * LINK_INT_STATUS register; otherwise the PCI_INT_P1394 will
	 * be set again immediately. */

	reg_write(lynx, PCI_INT_STATUS, pci_int_status);

	if ((pci_int_status & PCI_INT_DMA0_HLT) > 0) {
		packet_irq_handler(lynx);
		run_pcl(lynx, lynx->rcv_start_pcl_bus, 0);
	}

	return IRQ_HANDLED;
}