ff_layout_encode_ff_layoutupdate(struct xdr_stream *xdr,
			      const struct nfs42_layoutstat_devinfo *devinfo,
			      struct nfs4_ff_layout_mirror *mirror)
{
	struct nfs4_pnfs_ds_addr *da;
	struct nfs4_pnfs_ds *ds = mirror->mirror_ds->ds;
	struct nfs_fh *fh = &mirror->fh_versions[0];
	__be32 *p;

	da = list_first_entry(&ds->ds_addrs, struct nfs4_pnfs_ds_addr, da_node);
	dprintk("%s: DS %s: encoding address %s\n",
		__func__, ds->ds_remotestr, da->da_remotestr);
	/* netaddr4 */
	ff_layout_encode_netaddr(xdr, da);
	/* nfs_fh4 */
	p = xdr_reserve_space(xdr, 4 + fh->size);
	xdr_encode_opaque(p, fh->data, fh->size);
	/* ff_io_latency4 read */
	spin_lock(&mirror->lock);
	ff_layout_encode_io_latency(xdr, &mirror->read_stat.io_stat);
	/* ff_io_latency4 write */
	ff_layout_encode_io_latency(xdr, &mirror->write_stat.io_stat);
	spin_unlock(&mirror->lock);
	/* nfstime4 */
	ff_layout_encode_nfstime(xdr, ktime_sub(ktime_get(), mirror->start_time));
	/* bool */
	p = xdr_reserve_space(xdr, 4);
	*p = cpu_to_be32(false);
}