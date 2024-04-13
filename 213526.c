ff_layout_setup_ds_info(struct pnfs_ds_commit_info *fl_cinfo,
		struct pnfs_layout_segment *lseg)
{
	struct nfs4_ff_layout_segment *flseg = FF_LAYOUT_LSEG(lseg);
	struct inode *inode = lseg->pls_layout->plh_inode;
	struct pnfs_commit_array *array, *new;

	new = pnfs_alloc_commit_array(flseg->mirror_array_cnt, GFP_NOIO);
	if (new) {
		spin_lock(&inode->i_lock);
		array = pnfs_add_commit_array(fl_cinfo, new, lseg);
		spin_unlock(&inode->i_lock);
		if (array != new)
			pnfs_free_commit_array(new);
	}
}