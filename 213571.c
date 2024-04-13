ff_layout_pg_check_layout(struct nfs_pageio_descriptor *pgio,
			  struct nfs_page *req)
{
	pnfs_generic_pg_check_layout(pgio);
	pnfs_generic_pg_check_range(pgio, req);
}