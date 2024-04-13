virtio_set_modern_bar(struct virtio_base *base, bool use_notify_pio)
{
	struct virtio_ops *vops;
	int rc = 0;

	vops = base->vops;

	if (!vops || (base->device_caps & (1UL << VIRTIO_F_VERSION_1)) == 0)
		return -1;

	if (use_notify_pio)
		rc = virtio_set_modern_pio_bar(base,
			VIRTIO_MODERN_PIO_BAR_IDX);
	if (!rc)
		rc = virtio_set_modern_mmio_bar(base,
			VIRTIO_MODERN_MMIO_BAR_IDX);

	return rc;
}