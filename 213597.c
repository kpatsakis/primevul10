ff_layout_pg_get_read(struct nfs_pageio_descriptor *pgio,
		      struct nfs_page *req,
		      bool strict_iomode)
{
	pnfs_put_lseg(pgio->pg_lseg);
	pgio->pg_lseg = pnfs_update_layout(pgio->pg_inode,
					   nfs_req_openctx(req),
					   req_offset(req),
					   req->wb_bytes,
					   IOMODE_READ,
					   strict_iomode,
					   GFP_KERNEL);
	if (IS_ERR(pgio->pg_lseg)) {
		pgio->pg_error = PTR_ERR(pgio->pg_lseg);
		pgio->pg_lseg = NULL;
	}
}