ff_layout_alloc_layout_hdr(struct inode *inode, gfp_t gfp_flags)
{
	struct nfs4_flexfile_layout *ffl;

	ffl = kzalloc(sizeof(*ffl), gfp_flags);
	if (ffl) {
		pnfs_init_ds_commit_info(&ffl->commit_info);
		INIT_LIST_HEAD(&ffl->error_list);
		INIT_LIST_HEAD(&ffl->mirrors);
		ffl->last_report_time = ktime_get();
		ffl->commit_info.ops = &ff_layout_commit_ops;
		return &ffl->generic_hdr;
	} else
		return NULL;
}