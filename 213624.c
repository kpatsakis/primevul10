ff_layout_get_ds_info(struct inode *inode)
{
	struct pnfs_layout_hdr *layout = NFS_I(inode)->layout;

	if (layout == NULL)
		return NULL;

	return &FF_LAYOUT_FROM_HDR(layout)->commit_info;
}