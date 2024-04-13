nfs4_ff_layout_stat_io_update_requested(struct nfs4_ff_layoutstat *layoutstat,
		__u64 requested)
{
	struct nfs4_ff_io_stat *iostat = &layoutstat->io_stat;

	iostat->ops_requested++;
	iostat->bytes_requested += requested;
}