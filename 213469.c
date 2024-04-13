virtio_find_capability(struct virtio_base *base, uint8_t cfg_type)
{
	struct pci_vdev *dev = base->dev;
	uint8_t type;
	int rc, coff = 0;

	rc = pci_emul_find_capability(dev, PCIY_VENDOR, &coff);
	while (!rc) {
		type = pci_get_cfgdata8(dev,
			coff + offsetof(struct virtio_pci_cap, cfg_type));
		if (type == cfg_type)
			return coff;
		rc = pci_emul_find_capability(dev, PCIY_VENDOR, &coff);
	}

	return -1;
}