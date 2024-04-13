virtio_pci_read(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
		int baridx, uint64_t offset, int size)
{
	struct virtio_base *base = dev->arg;

	if (base->flags & VIRTIO_USE_MSIX) {
		if (baridx == pci_msix_table_bar(dev) ||
		    baridx == pci_msix_pba_bar(dev)) {
			return pci_emul_msix_tread(dev, offset, size);
		}
	}

	if (baridx == base->legacy_pio_bar_idx)
		return virtio_pci_legacy_read(ctx, vcpu, dev, baridx,
			offset, size);

	if (baridx == base->modern_mmio_bar_idx)
		return virtio_pci_modern_mmio_read(ctx, vcpu, dev, baridx,
			offset, size);

	if (baridx == base->modern_pio_bar_idx)
		return virtio_pci_modern_pio_read(ctx, vcpu, dev, baridx,
			offset, size);

	pr_err("%s: read unexpected baridx %d\r\n",
		base->vops->name, baridx);
	return size == 1 ? 0xff : size == 2 ? 0xffff : 0xffffffff;
}