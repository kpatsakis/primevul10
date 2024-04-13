virtio_set_io_bar(struct virtio_base *base, int barnum)
{
	size_t size;

	/*
	 * ??? should we use VIRTIO_PCI_CONFIG_OFF(0) if MSI-X
	 * is disabled? Existing code did not...
	 */
	size = VIRTIO_PCI_CONFIG_OFF(1) + base->vops->cfgsize;
	pci_emul_alloc_bar(base->dev, barnum, PCIBAR_IO, size);
	base->legacy_pio_bar_idx = barnum;
}