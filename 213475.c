virtio_interrupt_init(struct virtio_base *base, int use_msix)
{
	return virtio_intr_init(base, 1, use_msix);
}