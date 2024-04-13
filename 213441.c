virtio_pci_modern_pio_read(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
			   int baridx, uint64_t offset, int size)
{
	/* guest driver should not read notify pio */
	return 0;
}