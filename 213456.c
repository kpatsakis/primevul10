vq_endchains(struct virtio_vq_info *vq, int used_all_avail)
{
	struct virtio_base *base;
	uint16_t event_idx, new_idx, old_idx;
	int intr;

	if (!vq || !vq->used)
		return;

	/*
	 * Interrupt generation: if we're using EVENT_IDX,
	 * interrupt if we've crossed the event threshold.
	 * Otherwise interrupt is generated if we added "used" entries,
	 * but suppressed by VRING_AVAIL_F_NO_INTERRUPT.
	 *
	 * In any case, though, if NOTIFY_ON_EMPTY is set and the
	 * entire avail was processed, we need to interrupt always.
	 */

	atomic_thread_fence();

	base = vq->base;
	old_idx = vq->save_used;
	vq->save_used = new_idx = vq->used->idx;
	if (used_all_avail &&
	    (base->negotiated_caps & (1 << VIRTIO_F_NOTIFY_ON_EMPTY)))
		intr = 1;
	else if (base->negotiated_caps & (1 << VIRTIO_RING_F_EVENT_IDX)) {
		event_idx = VQ_USED_EVENT_IDX(vq);
		/*
		 * This calculation is per docs and the kernel
		 * (see src/sys/dev/virtio/virtio_ring.h).
		 */
		intr = (uint16_t)(new_idx - event_idx - 1) <
			(uint16_t)(new_idx - old_idx);
	} else {
		intr = new_idx != old_idx &&
		    !(vq->avail->flags & VRING_AVAIL_F_NO_INTERRUPT);
	}
	if (intr)
		vq_interrupt(base, vq);
}