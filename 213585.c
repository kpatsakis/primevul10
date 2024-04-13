ff_layout_encode_io_latency(struct xdr_stream *xdr,
			    struct nfs4_ff_io_stat *stat)
{
	__be32 *p;

	p = xdr_reserve_space(xdr, 5 * 8);
	p = xdr_encode_hyper(p, stat->ops_requested);
	p = xdr_encode_hyper(p, stat->bytes_requested);
	p = xdr_encode_hyper(p, stat->ops_completed);
	p = xdr_encode_hyper(p, stat->bytes_completed);
	p = xdr_encode_hyper(p, stat->bytes_not_delivered);
	ff_layout_encode_nfstime(xdr, stat->total_busy_time);
	ff_layout_encode_nfstime(xdr, stat->aggregate_completion_time);
}