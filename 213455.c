virtio_pci_modern_pio_write(struct vmctx *ctx, int vcpu, struct pci_vdev *dev,
			    int baridx, uint64_t offset, int size,
			    uint64_t value)
{
	struct virtio_base *base = dev->arg;
	struct virtio_vq_info *vq;
	struct virtio_ops *vops;
	const char *name;
	uint64_t idx;


	vops = base->vops;
	name = vops->name;
	idx = value;

	if (size != 1 && size != 2 && size != 4) {
		pr_err("%s: write to [%d:0x%lx] bad size %d\r\n",
			name, baridx, offset, size);
		return;
	}

	if (idx >= vops->nvq) {
		pr_err("%s: queue %lu notify out of range\r\n", name, idx);
		return;
	}

	if (base->mtx)
		pthread_mutex_lock(base->mtx);

	vq = &base->queues[idx];
	if (vq->notify)
		(*vq->notify)(DEV_STRUCT(base), vq);
	else if (vops->qnotify)
		(*vops->qnotify)(DEV_STRUCT(base), vq);
	else
		pr_err("%s: qnotify queue %lu: missing vq/vops notify\r\n",
			name, idx);

	if (base->mtx)
		pthread_mutex_unlock(base->mtx);
}