virtio_set_modern_pio_bar(struct virtio_base *base, int barnum)
{
	int rc;
	struct virtio_pci_notify_cap notify_pio = {
		.cap.cap_vndr = PCIY_VENDOR,
		.cap.cap_next = 0,
		.cap.cap_len = sizeof(notify_pio),
		.cap.cfg_type = VIRTIO_PCI_CAP_NOTIFY_CFG,
		.cap.bar = barnum,
		.cap.offset = 0,
		.cap.length = 4,
		.notify_off_multiplier = 0,
	};

	/* notification capability */
	rc = pci_emul_add_capability(base->dev, (u_char *)&notify_pio,
		sizeof(notify_pio));
	if (rc != 0) {
		pr_err("pci emulation add notification capability for virtio modern PIO BAR failed\n");
		return -1;
	}

	/* allocate and register modern pio bar */
	rc = pci_emul_alloc_bar(base->dev, barnum, PCIBAR_IO, 4);
	if (rc != 0) {
		pr_err("allocate and register modern pio bar failed\n");
		return -1;
	}

	base->modern_pio_bar_idx = barnum;
	return 0;
}