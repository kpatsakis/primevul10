virtio_set_modern_mmio_bar(struct virtio_base *base, int barnum)
{
	struct virtio_ops *vops;
	int rc;
	struct virtio_pci_cap cap = {
		.cap_vndr = PCIY_VENDOR,
		.cap_next = 0,
		.cap_len = sizeof(cap),
		.bar = barnum,
	};
	struct virtio_pci_notify_cap notify = {
		.cap.cap_vndr = PCIY_VENDOR,
		.cap.cap_next = 0,
		.cap.cap_len = sizeof(notify),
		.cap.cfg_type = VIRTIO_PCI_CAP_NOTIFY_CFG,
		.cap.bar = barnum,
		.cap.offset = VIRTIO_CAP_NOTIFY_OFFSET,
		.cap.length = VIRTIO_CAP_NOTIFY_SIZE,
		.notify_off_multiplier = VIRTIO_MODERN_NOTIFY_OFF_MULT,
	};
	struct virtio_pci_cfg_cap cfg = {
		.cap.cap_vndr = PCIY_VENDOR,
		.cap.cap_next = 0,
		.cap.cap_len = sizeof(cfg),
		.cap.cfg_type = VIRTIO_PCI_CAP_PCI_CFG,
	};

	vops = base->vops;

	if (vops->cfgsize > VIRTIO_CAP_DEVICE_SIZE) {
		pr_err("%s: cfgsize %lu > max %d\r\n",
			vops->name, vops->cfgsize, VIRTIO_CAP_DEVICE_SIZE);
		return -1;
	}

	/* common configuration capability */
	cap.cfg_type = VIRTIO_PCI_CAP_COMMON_CFG;
	cap.offset = VIRTIO_CAP_COMMON_OFFSET;
	cap.length = VIRTIO_CAP_COMMON_SIZE;
	rc = pci_emul_add_capability(base->dev, (u_char *)&cap, sizeof(cap));
	if (rc != 0) {
		pr_err("pci emulation add common configuration capability failed\n");
		return -1;
	}

	/* isr status capability */
	cap.cfg_type = VIRTIO_PCI_CAP_ISR_CFG;
	cap.offset = VIRTIO_CAP_ISR_OFFSET;
	cap.length = VIRTIO_CAP_ISR_SIZE;
	rc = pci_emul_add_capability(base->dev, (u_char *)&cap, sizeof(cap));
	if (rc != 0) {
		pr_err("pci emulation add isr status capability failed\n");
		return -1;
	}

	/* device specific configuration capability */
	cap.cfg_type = VIRTIO_PCI_CAP_DEVICE_CFG;
	cap.offset = VIRTIO_CAP_DEVICE_OFFSET;
	cap.length = VIRTIO_CAP_DEVICE_SIZE;
	rc = pci_emul_add_capability(base->dev, (u_char *)&cap, sizeof(cap));
	if (rc != 0) {
		pr_err("pci emulation add device specific configuration capability failed\n");
		return -1;
	}

	/* notification capability */
	rc = pci_emul_add_capability(base->dev, (u_char *)&notify,
		sizeof(notify));
	if (rc != 0) {
		pr_err("pci emulation add notification capability failed\n");
		return -1;
	}

	/* pci alternative configuration access capability */
	rc = pci_emul_add_capability(base->dev, (u_char *)&cfg, sizeof(cfg));
	if (rc != 0) {
		pr_err("pci emulation add alternative configuration access capability failed\n");
		return -1;
	}

	/* allocate and register modern memory bar */
	rc = pci_emul_alloc_bar(base->dev, barnum, PCIBAR_MEM64,
				VIRTIO_MODERN_MEM_BAR_SIZE);
	if (rc != 0) {
		pr_err("allocate and register modern memory bar failed\n");
		return -1;
	}

	base->cfg_coff = virtio_find_capability(base, VIRTIO_PCI_CAP_PCI_CFG);
	if (base->cfg_coff < 0) {
		pr_err("%s: VIRTIO_PCI_CAP_PCI_CFG not found\r\n",
			vops->name);
		return -1;
	}

	base->modern_mmio_bar_idx = barnum;
	return 0;
}