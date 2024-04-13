virtio_pci_write(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
		 int baridx, uint64_t offset, int size, uint64_t value)
{
	struct virtio_base *base = dev->arg;

	if (base->flags & VIRTIO_USE_MSIX) {
		if (baridx == pci_msix_table_bar(dev) ||
		    baridx == pci_msix_pba_bar(dev)) {
			pci_emul_msix_twrite(dev, offset, size, value);
			return;
		}
	}

	if (baridx == base->legacy_pio_bar_idx) {
		virtio_pci_legacy_write(ctx, vcpu, dev, baridx,
			offset, size, value);
		return;
	}

	if (baridx == base->modern_mmio_bar_idx) {
		virtio_pci_modern_mmio_write(ctx, vcpu, dev, baridx,
			offset, size, value);
		return;
	}

	if (baridx == base->modern_pio_bar_idx) {
		virtio_pci_modern_pio_write(ctx, vcpu, dev, baridx,
			offset, size, value);
		return;
	}

	pr_err("%s: write unexpected baridx %d\r\n",
		base->vops->name, baridx);
}