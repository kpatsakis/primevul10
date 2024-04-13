u32 agp_collect_device_status(struct agp_bridge_data *bridge, u32 requested_mode, u32 bridge_agpstat)
{
	struct pci_dev *device = NULL;
	u32 vga_agpstat;
	u8 cap_ptr;

	for (;;) {
		device = pci_get_class(PCI_CLASS_DISPLAY_VGA << 8, device);
		if (!device) {
			printk(KERN_INFO PFX "Couldn't find an AGP VGA controller.\n");
			return 0;
		}
		cap_ptr = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (cap_ptr)
			break;
	}

	/*
	 * Ok, here we have a AGP device. Disable impossible
	 * settings, and adjust the readqueue to the minimum.
	 */
	pci_read_config_dword(device, cap_ptr+PCI_AGP_STATUS, &vga_agpstat);

	/* adjust RQ depth */
	bridge_agpstat = ((bridge_agpstat & ~AGPSTAT_RQ_DEPTH) |
	     min_t(u32, (requested_mode & AGPSTAT_RQ_DEPTH),
		 min_t(u32, (bridge_agpstat & AGPSTAT_RQ_DEPTH), (vga_agpstat & AGPSTAT_RQ_DEPTH))));

	/* disable FW if it's not supported */
	if (!((bridge_agpstat & AGPSTAT_FW) &&
		 (vga_agpstat & AGPSTAT_FW) &&
		 (requested_mode & AGPSTAT_FW)))
		bridge_agpstat &= ~AGPSTAT_FW;

	/* Check to see if we are operating in 3.0 mode */
	if (agp_bridge->mode & AGPSTAT_MODE_3_0)
		agp_v3_parse_one(&requested_mode, &bridge_agpstat, &vga_agpstat);
	else
		agp_v2_parse_one(&requested_mode, &bridge_agpstat, &vga_agpstat);

	pci_dev_put(device);
	return bridge_agpstat;
}