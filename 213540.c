ff_layout_set_layoutdriver(struct nfs_server *server,
		const struct nfs_fh *dummy)
{
#if IS_ENABLED(CONFIG_NFS_V4_2)
	server->caps |= NFS_CAP_LAYOUTSTATS;
#endif
	return 0;
}