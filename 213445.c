_vq_record(int i, volatile struct vring_desc *vd, struct vmctx *ctx,
	   struct iovec *iov, int n_iov, uint16_t *flags) {

	void *host_addr;

	if (i >= n_iov)
		return -1;
	host_addr = paddr_guest2host(ctx, vd->addr, vd->len);
	if (!host_addr)
		return -1;
	iov[i].iov_base = host_addr;
	iov[i].iov_len = vd->len;
	if (flags != NULL)
		flags[i] = vd->flags;
	return 0;
}