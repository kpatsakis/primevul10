virtio_intr_init(struct virtio_base *base, int barnum, int use_msix)
{
	int nvec;

	if (use_msix) {
		base->flags |= VIRTIO_USE_MSIX;
		VIRTIO_BASE_LOCK(base);
		virtio_reset_dev(base); /* set all vectors to NO_VECTOR */
		VIRTIO_BASE_UNLOCK(base);
		nvec = base->vops->nvq + 1;
		if (pci_emul_add_msixcap(base->dev, nvec, barnum))
			return -1;
	} else
		base->flags &= ~VIRTIO_USE_MSIX;

	/* Only 1 MSI vector for acrn-dm */
	pci_emul_add_msicap(base->dev, 1);

	/* Legacy interrupts are mandatory for virtio devices */
	pci_lintr_request(base->dev);

	return 0;
}