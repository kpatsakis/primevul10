void agp_device_command(u32 bridge_agpstat, bool agp_v3)
{
	struct pci_dev *device = NULL;
	int mode;

	mode = bridge_agpstat & 0x7;
	if (agp_v3)
		mode *= 4;

	for_each_pci_dev(device) {
		u8 agp = pci_find_capability(device, PCI_CAP_ID_AGP);
		if (!agp)
			continue;

		dev_info(&device->dev, "putting AGP V%d device into %dx mode\n",
			 agp_v3 ? 3 : 2, mode);
		pci_write_config_dword(device, agp + PCI_AGP_COMMAND, bridge_agpstat);
	}
}