ff_layout_pg_init_write(struct nfs_pageio_descriptor *pgio,
			struct nfs_page *req)
{
	struct nfs4_ff_layout_mirror *mirror;
	struct nfs_pgio_mirror *pgm;
	struct nfs4_pnfs_ds *ds;
	u32 i;

retry:
	ff_layout_pg_check_layout(pgio, req);
	if (!pgio->pg_lseg) {
		pgio->pg_lseg = pnfs_update_layout(pgio->pg_inode,
						   nfs_req_openctx(req),
						   req_offset(req),
						   req->wb_bytes,
						   IOMODE_RW,
						   false,
						   GFP_NOFS);
		if (IS_ERR(pgio->pg_lseg)) {
			pgio->pg_error = PTR_ERR(pgio->pg_lseg);
			pgio->pg_lseg = NULL;
			return;
		}
	}
	/* If no lseg, fall back to write through mds */
	if (pgio->pg_lseg == NULL)
		goto out_mds;

	/* Use a direct mapping of ds_idx to pgio mirror_idx */
	if (pgio->pg_mirror_count != FF_LAYOUT_MIRROR_COUNT(pgio->pg_lseg))
		goto out_eagain;

	for (i = 0; i < pgio->pg_mirror_count; i++) {
		mirror = FF_LAYOUT_COMP(pgio->pg_lseg, i);
		ds = nfs4_ff_layout_prepare_ds(pgio->pg_lseg, mirror, true);
		if (!ds) {
			if (!ff_layout_no_fallback_to_mds(pgio->pg_lseg))
				goto out_mds;
			pnfs_generic_pg_cleanup(pgio);
			/* Sleep for 1 second before retrying */
			ssleep(1);
			goto retry;
		}
		pgm = &pgio->pg_mirrors[i];
		pgm->pg_bsize = mirror->mirror_ds->ds_versions[0].wsize;
	}

	if (NFS_SERVER(pgio->pg_inode)->flags &
			(NFS_MOUNT_SOFT|NFS_MOUNT_SOFTERR))
		pgio->pg_maxretrans = io_maxretrans;
	return;
out_eagain:
	pnfs_generic_pg_cleanup(pgio);
	pgio->pg_error = -EAGAIN;
	return;
out_mds:
	trace_pnfs_mds_fallback_pg_init_write(pgio->pg_inode,
			0, NFS4_MAX_UINT64, IOMODE_RW,
			NFS_I(pgio->pg_inode)->layout,
			pgio->pg_lseg);
	pgio->pg_maxretrans = 0;
	nfs_pageio_reset_write_mds(pgio);
	pgio->pg_error = -EAGAIN;
}