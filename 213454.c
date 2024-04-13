virtio_pci_legacy_read(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
		       int baridx, uint64_t offset, int size)
{
	struct virtio_base *base = dev->arg;
	struct virtio_ops *vops;
	const struct config_reg *cr;
	uint64_t virtio_config_size, max;
	const char *name;
	uint32_t newoff;
	uint32_t value;
	int error = -1;


	if (base->mtx)
		pthread_mutex_lock(base->mtx);

	vops = base->vops;
	name = vops->name;
	value = size == 1 ? 0xff : size == 2 ? 0xffff : 0xffffffff;

	if (size != 1 && size != 2 && size != 4)
		goto bad;

	if (pci_msix_enabled(dev))
		virtio_config_size = VIRTIO_PCI_CONFIG_OFF(1);
	else
		virtio_config_size = VIRTIO_PCI_CONFIG_OFF(0);

	if (offset >= virtio_config_size) {
		/*
		 * Subtract off the standard size (including MSI-X
		 * registers if enabled) and dispatch to underlying driver.
		 * If that fails, fall into general code.
		 */
		newoff = offset - virtio_config_size;
		max = vops->cfgsize ? vops->cfgsize : 0x100000000;
		if (newoff + size > max)
			goto bad;

		if (vops->cfgread) {
			error = (*vops->cfgread)(DEV_STRUCT(base), newoff,
						 size, &value);
		}
		if (!error)
			goto done;
	}

bad:
	cr = virtio_find_legacy_cr(offset);
	if (cr == NULL || cr->size != size) {
		if (cr != NULL) {
			/* offset must be OK, so size must be bad */
			pr_err("%s: read from %s: bad size %d\r\n",
			    name, cr->name, size);
		} else {
			pr_err("%s: read from bad offset/size %jd/%d\r\n",
			    name, (uintmax_t)offset, size);
		}
		goto done;
	}

	switch (offset) {
	case VIRTIO_PCI_HOST_FEATURES:
		value = base->device_caps;
		break;
	case VIRTIO_PCI_GUEST_FEATURES:
		value = base->negotiated_caps;
		break;
	case VIRTIO_PCI_QUEUE_PFN:
		if (base->curq < vops->nvq)
			value = base->queues[base->curq].pfn;
		break;
	case VIRTIO_PCI_QUEUE_NUM:
		value = base->curq < vops->nvq ?
			base->queues[base->curq].qsize : 0;
		break;
	case VIRTIO_PCI_QUEUE_SEL:
		value = base->curq;
		break;
	case VIRTIO_PCI_QUEUE_NOTIFY:
		value = 0;	/* XXX */
		break;
	case VIRTIO_PCI_STATUS:
		value = base->status;
		break;
	case VIRTIO_PCI_ISR:
		value = base->isr;
		base->isr = 0;		/* a read clears this flag */
		if (value)
			pci_lintr_deassert(dev);
		break;
	case VIRTIO_MSI_CONFIG_VECTOR:
		value = base->msix_cfg_idx;
		break;
	case VIRTIO_MSI_QUEUE_VECTOR:
		value = base->curq < vops->nvq ?
		    base->queues[base->curq].msix_idx :
		    VIRTIO_MSI_NO_VECTOR;
		break;
	}
done:
	if (base->mtx)
		pthread_mutex_unlock(base->mtx);
	return value;
}