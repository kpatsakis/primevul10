ff_layout_alloc_deviceid_node(struct nfs_server *server,
			      struct pnfs_device *pdev, gfp_t gfp_flags)
{
	struct nfs4_ff_layout_ds *dsaddr;

	dsaddr = nfs4_ff_alloc_deviceid_node(server, pdev, gfp_flags);
	if (!dsaddr)
		return NULL;
	return &dsaddr->id_node;
}