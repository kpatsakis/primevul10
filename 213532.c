ff_layout_read_pagelist(struct nfs_pgio_header *hdr)
{
	struct pnfs_layout_segment *lseg = hdr->lseg;
	struct nfs4_pnfs_ds *ds;
	struct rpc_clnt *ds_clnt;
	struct nfs4_ff_layout_mirror *mirror;
	const struct cred *ds_cred;
	loff_t offset = hdr->args.offset;
	u32 idx = hdr->pgio_mirror_idx;
	int vers;
	struct nfs_fh *fh;

	dprintk("--> %s ino %lu pgbase %u req %zu@%llu\n",
		__func__, hdr->inode->i_ino,
		hdr->args.pgbase, (size_t)hdr->args.count, offset);

	mirror = FF_LAYOUT_COMP(lseg, idx);
	ds = nfs4_ff_layout_prepare_ds(lseg, mirror, false);
	if (!ds)
		goto out_failed;

	ds_clnt = nfs4_ff_find_or_create_ds_client(mirror, ds->ds_clp,
						   hdr->inode);
	if (IS_ERR(ds_clnt))
		goto out_failed;

	ds_cred = ff_layout_get_ds_cred(mirror, &lseg->pls_range, hdr->cred);
	if (!ds_cred)
		goto out_failed;

	vers = nfs4_ff_layout_ds_version(mirror);

	dprintk("%s USE DS: %s cl_count %d vers %d\n", __func__,
		ds->ds_remotestr, refcount_read(&ds->ds_clp->cl_count), vers);

	hdr->pgio_done_cb = ff_layout_read_done_cb;
	refcount_inc(&ds->ds_clp->cl_count);
	hdr->ds_clp = ds->ds_clp;
	fh = nfs4_ff_layout_select_ds_fh(mirror);
	if (fh)
		hdr->args.fh = fh;

	nfs4_ff_layout_select_ds_stateid(mirror, &hdr->args.stateid);

	/*
	 * Note that if we ever decide to split across DSes,
	 * then we may need to handle dense-like offsets.
	 */
	hdr->args.offset = offset;
	hdr->mds_offset = offset;

	/* Perform an asynchronous read to ds */
	nfs_initiate_pgio(ds_clnt, hdr, ds_cred, ds->ds_clp->rpc_ops,
			  vers == 3 ? &ff_layout_read_call_ops_v3 :
				      &ff_layout_read_call_ops_v4,
			  0, RPC_TASK_SOFTCONN);
	put_cred(ds_cred);
	return PNFS_ATTEMPTED;

out_failed:
	if (ff_layout_avoid_mds_available_ds(lseg))
		return PNFS_TRY_AGAIN;
	trace_pnfs_mds_fallback_read_pagelist(hdr->inode,
			hdr->args.offset, hdr->args.count,
			IOMODE_READ, NFS_I(hdr->inode)->layout, lseg);
	return PNFS_NOT_ATTEMPTED;
}