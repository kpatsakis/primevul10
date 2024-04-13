ff_layout_encode_nfstime(struct xdr_stream *xdr,
			 ktime_t t)
{
	struct timespec64 ts;
	__be32 *p;

	p = xdr_reserve_space(xdr, 12);
	ts = ktime_to_timespec64(t);
	p = xdr_encode_hyper(p, ts.tv_sec);
	*p++ = cpu_to_be32(ts.tv_nsec);
}