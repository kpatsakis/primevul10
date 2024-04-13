virtio_poll_timer(void *arg, uint64_t nexp)
{
	struct virtio_base *base;
	struct virtio_ops *vops;
	struct virtio_vq_info *vq;
	const char *name;
	int i;

	base = arg;
	vops = base->vops;
	name = vops->name;

	if (base->mtx)
		pthread_mutex_lock(base->mtx);

	base->polling_in_progress = 1;

	for (i = 0; i < base->vops->nvq; i++) {
		vq = &base->queues[i];
		if(!vq_ring_ready(vq))
			continue;
		vq->used->flags |= VRING_USED_F_NO_NOTIFY;
		/* TODO: call notify when necessary */
		if (vq->notify)
			(*vq->notify)(DEV_STRUCT(base), vq);
		else if (vops->qnotify)
			(*vops->qnotify)(DEV_STRUCT(base), vq);
		else
			pr_err("%s: qnotify queue %d: missing vq/vops notify\r\n",
				name, i);
	}

	if (base->mtx)
		pthread_mutex_unlock(base->mtx);

	virtio_start_timer(&base->polling_timer, 0, virtio_poll_interval);
}