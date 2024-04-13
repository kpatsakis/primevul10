virtio_vq_enable(struct virtio_base *base)
{
	struct virtio_vq_info *vq;
	uint16_t qsz;
	uint64_t phys;
	size_t size;
	char *vb;

	vq = &base->queues[base->curq];
	qsz = vq->qsize;

	/* descriptors */
	phys = (((uint64_t)vq->gpa_desc[1]) << 32) | vq->gpa_desc[0];
	size = qsz * sizeof(struct vring_desc);
	vb = paddr_guest2host(base->dev->vmctx, phys, size);
	if (!vb)
		goto error;
	vq->desc = (struct vring_desc *)vb;

	/* available ring */
	phys = (((uint64_t)vq->gpa_avail[1]) << 32) | vq->gpa_avail[0];
	size = (2 + qsz + 1) * sizeof(uint16_t);
	vb = paddr_guest2host(base->dev->vmctx, phys, size);
	if (!vb)
		goto error;

	vq->avail = (struct vring_avail *)vb;

	/* used ring */
	phys = (((uint64_t)vq->gpa_used[1]) << 32) | vq->gpa_used[0];
	size = sizeof(uint16_t) * 3 + sizeof(struct vring_used_elem) * qsz;
	vb = paddr_guest2host(base->dev->vmctx, phys, size);
	if (!vb)
		goto error;
	vq->used = (struct vring_used *)vb;

	/* Start at 0 when we use it. */
	vq->last_avail = 0;
	vq->save_used = 0;

	/* Mark queue as enabled. */
	vq->enabled = true;

	/* Mark queue as allocated after initialization is complete. */
	mb();
	vq->flags = VQ_ALLOC;
	return;
 error:
	vq->flags = 0;
	pr_err("%s: vq enable failed\n", __func__);
}