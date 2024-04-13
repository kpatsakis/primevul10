ff_layout_pg_init_read(struct nfs_pageio_descriptor *pgio,
			struct nfs_page *req)
{
	struct nfs_pgio_mirror *pgm;
	struct nfs4_ff_layout_mirror *mirror;
	struct nfs4_pnfs_ds *ds;
	u32 ds_idx;

retry:
	ff_layout_pg_check_layout(pgio, req);
	/* Use full layout for now */
	if (!pgio->pg_lseg) {
		ff_layout_pg_get_read(pgio, req, false);
		if (!pgio->pg_lseg)
			goto out_nolseg;
	}
	if (ff_layout_avoid_read_on_rw(pgio->pg_lseg)) {
		ff_layout_pg_get_read(pgio, req, true);
		if (!pgio->pg_lseg)
			goto out_nolseg;
	}

	ds = ff_layout_get_ds_for_read(pgio, &ds_idx);
	if (!ds) {
		if (!ff_layout_no_fallback_to_mds(pgio->pg_lseg))
			goto out_mds;
		pnfs_generic_pg_cleanup(pgio);
		/* Sleep for 1 second before retrying */
		ssleep(1);
		goto retry;
	}

	mirror = FF_LAYOUT_COMP(pgio->pg_lseg, ds_idx);
	pgm = &pgio->pg_mirrors[0];
	pgm->pg_bsize = mirror->mirror_ds->ds_versions[0].rsize;

	pgio->pg_mirror_idx = ds_idx;

	if (NFS_SERVER(pgio->pg_inode)->flags &
			(NFS_MOUNT_SOFT|NFS_MOUNT_SOFTERR))
		pgio->pg_maxretrans = io_maxretrans;
	return;
out_nolseg:
	if (pgio->pg_error < 0)
		return;
out_mds:
	trace_pnfs_mds_fallback_pg_init_read(pgio->pg_inode,
			0, NFS4_MAX_UINT64, IOMODE_READ,
			NFS_I(pgio->pg_inode)->layout,
			pgio->pg_lseg);
	pgio->pg_maxretrans = 0;
	nfs_pageio_reset_read_mds(pgio);
}