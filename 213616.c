ff_layout_release_ds_info(struct pnfs_ds_commit_info *fl_cinfo,
		struct inode *inode)
{
	spin_lock(&inode->i_lock);
	pnfs_generic_ds_cinfo_destroy(fl_cinfo);
	spin_unlock(&inode->i_lock);
}