ff_layout_pg_get_mirror_count_write(struct nfs_pageio_descriptor *pgio,
				    struct nfs_page *req)
{
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
			goto out;
		}
	}
	if (pgio->pg_lseg)
		return FF_LAYOUT_MIRROR_COUNT(pgio->pg_lseg);

	trace_pnfs_mds_fallback_pg_get_mirror_count(pgio->pg_inode,
			0, NFS4_MAX_UINT64, IOMODE_RW,
			NFS_I(pgio->pg_inode)->layout,
			pgio->pg_lseg);
	/* no lseg means that pnfs is not in use, so no mirroring here */
	nfs_pageio_reset_write_mds(pgio);
out:
	return 1;
}