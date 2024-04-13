virtio_pci_modern_mmio_write(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
			     int baridx, uint64_t offset, int size,
			     uint64_t value)
{
	struct virtio_base *base = dev->arg;
	struct virtio_ops *vops;
	const char *name;
	int capid;


	vops = base->vops;
	name = vops->name;

	if (size != 1 && size != 2 && size != 4) {
		pr_err("%s: write to [%d:0x%lx] bad size %d\r\n",
			name, baridx, offset, size);
		return;
	}

	capid = virtio_get_cap_id(offset, size);
	if (capid < 0) {
		pr_err("%s: write to [%d:0x%lx] bad range %d\r\n",
			name, baridx, offset, size);
		return;
	}

	if (base->mtx)
		pthread_mutex_lock(base->mtx);

	switch (capid) {
	case VIRTIO_PCI_CAP_COMMON_CFG:
		offset -= VIRTIO_CAP_COMMON_OFFSET;
		virtio_common_cfg_write(dev, offset, size, value);
		break;
	case VIRTIO_PCI_CAP_DEVICE_CFG:
		offset -= VIRTIO_CAP_DEVICE_OFFSET;
		virtio_device_cfg_write(dev, offset, size, value);
		break;
	case VIRTIO_PCI_CAP_NOTIFY_CFG:
		offset -= VIRTIO_CAP_NOTIFY_OFFSET;
		virtio_notify_cfg_write(dev, offset, size, value);
		break;
	default: /* guest driver should not write to ISR region */
		pr_err("%s: write to [%d:0x%lx] size %d not supported\r\n",
			name, baridx, offset, size);
	}

	if (base->mtx)
		pthread_mutex_unlock(base->mtx);
}