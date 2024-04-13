virtio_pci_modern_mmio_read(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
			    int baridx, uint64_t offset, int size)
{
	struct virtio_base *base = dev->arg;
	struct virtio_ops *vops;
	const char *name;
	uint32_t value;
	int capid;


	vops = base->vops;
	name = vops->name;
	value = size == 1 ? 0xff : size == 2 ? 0xffff : 0xffffffff;

	if (size != 1 && size != 2 && size != 4) {
		pr_err("%s: read from [%d:0x%lx] bad size %d\r\n",
			name, baridx, offset, size);
		return value;
	}

	capid = virtio_get_cap_id(offset, size);
	if (capid < 0) {
		pr_err("%s: read from [%d:0x%lx] bad range %d\r\n",
			name, baridx, offset, size);
		return value;
	}

	if (base->mtx)
		pthread_mutex_lock(base->mtx);

	switch (capid) {
	case VIRTIO_PCI_CAP_COMMON_CFG:
		offset -= VIRTIO_CAP_COMMON_OFFSET;
		value = virtio_common_cfg_read(dev, offset, size);
		break;
	case VIRTIO_PCI_CAP_ISR_CFG:
		offset -= VIRTIO_CAP_ISR_OFFSET;
		value = virtio_isr_cfg_read(dev, offset, size);
		break;
	case VIRTIO_PCI_CAP_DEVICE_CFG:
		offset -= VIRTIO_CAP_DEVICE_OFFSET;
		value = virtio_device_cfg_read(dev, offset, size);
		break;
	default: /* guest driver should not read from notify region */
		pr_err("%s: read from [%d:0x%lx] size %d not supported\r\n",
			name, baridx, offset, size);
	}

	if (base->mtx)
		pthread_mutex_unlock(base->mtx);
	return value;
}