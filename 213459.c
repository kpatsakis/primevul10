virtio_notify_cfg_write(struct pci_vdev *dev, uint64_t offset, int size,
			uint64_t value)
{
	struct virtio_base *base = dev->arg;
	struct virtio_vq_info *vq;
	struct virtio_ops *vops;
	const char *name;
	uint64_t idx;

	idx = offset / VIRTIO_MODERN_NOTIFY_OFF_MULT;
	vops = base->vops;
	name = vops->name;

	if (idx >= vops->nvq) {
		pr_err("%s: queue %lu notify out of range\r\n", name, idx);
		return;
	}

	vq = &base->queues[idx];
	if (vq->notify)
		(*vq->notify)(DEV_STRUCT(base), vq);
	else if (vops->qnotify)
		(*vops->qnotify)(DEV_STRUCT(base), vq);
	else
		pr_err("%s: qnotify queue %lu: missing vq/vops notify\r\n",
			name, idx);
}