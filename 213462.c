virtio_isr_cfg_read(struct pci_vdev *dev, uint64_t offset, int size)
{
	struct virtio_base *base = dev->arg;
	uint32_t value = 0;

	value = base->isr;
	base->isr = 0;		/* a read clears this flag */
	if (value)
		pci_lintr_deassert(dev);

	return value;
}