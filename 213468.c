virtio_vq_init(struct virtio_base *base, uint32_t pfn)
{
	struct virtio_vq_info *vq;
	uint64_t phys;
	size_t size;
	char *vb;

	vq = &base->queues[base->curq];
	vq->pfn = pfn;
	phys = (uint64_t)pfn << VRING_PAGE_BITS;
	size = vring_size(vq->qsize, VIRTIO_PCI_VRING_ALIGN);
	vb = paddr_guest2host(base->dev->vmctx, phys, size);
	if (!vb)
		goto error;

	/* First page(s) are descriptors... */
	vq->desc = (struct vring_desc *)vb;
	vb += vq->qsize * sizeof(struct vring_desc);

	/* ... immediately followed by "avail" ring (entirely uint16_t's) */
	vq->avail = (struct vring_avail *)vb;
	vb += (2 + vq->qsize + 1) * sizeof(uint16_t);

	/* Then it's rounded up to the next page... */
	vb = (char *)roundup2((uintptr_t)vb, VIRTIO_PCI_VRING_ALIGN);

	/* ... and the last page(s) are the used ring. */
	vq->used = (struct vring_used *)vb;

	/* Start at 0 when we use it. */
	vq->last_avail = 0;
	vq->save_used = 0;

	/* Mark queue as allocated after initialization is complete. */
	mb();
	vq->flags = VQ_ALLOC;

	return;

error:
	vq->flags = 0;
	pr_err("%s: vq enable failed\n", __func__);
}