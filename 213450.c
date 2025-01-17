vq_getchain(struct virtio_vq_info *vq, uint16_t *pidx,
	    struct iovec *iov, int n_iov, uint16_t *flags)
{
	int i;
	u_int ndesc, n_indir;
	u_int idx, next;

	volatile struct vring_desc *vdir, *vindir, *vp;
	struct vmctx *ctx;
	struct virtio_base *base;
	const char *name;

	base = vq->base;
	name = base->vops->name;

	/*
	 * Note: it's the responsibility of the guest not to
	 * update vq->avail->idx until all of the descriptors
	 * the guest has written are valid (including all their
	 * next fields and vd_flags).
	 *
	 * Compute (last_avail - idx) in integers mod 2**16.  This is
	 * the number of descriptors the device has made available
	 * since the last time we updated vq->last_avail.
	 *
	 * We just need to do the subtraction as an unsigned int,
	 * then trim off excess bits.
	 */
	idx = vq->last_avail;
	ndesc = (uint16_t)((u_int)vq->avail->idx - idx);
	if (ndesc == 0)
		return 0;
	if (ndesc > vq->qsize) {
		/* XXX need better way to diagnose issues */
		pr_err("%s: ndesc (%u) out of range, driver confused?\r\n",
		    name, (u_int)ndesc);
		return -1;
	}

	/*
	 * Now count/parse "involved" descriptors starting from
	 * the head of the chain.
	 *
	 * To prevent loops, we could be more complicated and
	 * check whether we're re-visiting a previously visited
	 * index, but we just abort if the count gets excessive.
	 */
	ctx = base->dev->vmctx;
	*pidx = next = vq->avail->ring[idx & (vq->qsize - 1)];
	vq->last_avail++;
	for (i = 0; i < VQ_MAX_DESCRIPTORS; next = vdir->next) {
		if (next >= vq->qsize) {
			pr_err("%s: descriptor index %u out of range, "
			    "driver confused?\r\n",
			    name, next);
			return -1;
		}
		vdir = &vq->desc[next];
		if ((vdir->flags & VRING_DESC_F_INDIRECT) == 0) {
			if (_vq_record(i, vdir, ctx, iov, n_iov, flags)) {
				pr_err("%s: mapping to host failed\r\n", name);
				return -1;
			}
			i++;
		} else if ((base->device_caps &
		    (1 << VIRTIO_RING_F_INDIRECT_DESC)) == 0) {
			pr_err("%s: descriptor has forbidden INDIRECT flag, "
			    "driver confused?\r\n",
			    name);
			return -1;
		} else {
			n_indir = vdir->len / 16;
			if ((vdir->len & 0xf) || n_indir == 0) {
				pr_err("%s: invalid indir len 0x%x, "
				    "driver confused?\r\n",
				    name, (u_int)vdir->len);
				return -1;
			}
			vindir = paddr_guest2host(ctx,
			    vdir->addr, vdir->len);

			if (!vindir) {
				pr_err("%s cannot get host memory\r\n", name);
				return -1;
			}
			/*
			 * Indirects start at the 0th, then follow
			 * their own embedded "next"s until those run
			 * out.  Each one's indirect flag must be off
			 * (we don't really have to check, could just
			 * ignore errors...).
			 */
			next = 0;
			for (;;) {
				vp = &vindir[next];
				if (vp->flags & VRING_DESC_F_INDIRECT) {
					pr_err("%s: indirect desc has INDIR flag,"
					    " driver confused?\r\n",
					    name);
					return -1;
				}
				if (_vq_record(i, vp, ctx, iov, n_iov, flags)) {
					pr_err("%s: mapping to host failed\r\n", name);
					return -1;
				}
				if (++i > VQ_MAX_DESCRIPTORS)
					goto loopy;
				if ((vp->flags & VRING_DESC_F_NEXT) == 0)
					break;
				next = vp->next;
				if (next >= n_indir) {
					pr_err("%s: invalid next %u > %u, "
					    "driver confused?\r\n",
					    name, (u_int)next, n_indir);
					return -1;
				}
			}
		}
		if ((vdir->flags & VRING_DESC_F_NEXT) == 0)
			return i;
	}
loopy:
	pr_err("%s: descriptor loop? count > %d - driver confused?\r\n",
	    name, i);
	return -1;
}