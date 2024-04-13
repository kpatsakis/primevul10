ff_layout_commit_pagelist(struct inode *inode, struct list_head *mds_pages,
			   int how, struct nfs_commit_info *cinfo)
{
	return pnfs_generic_commit_pagelist(inode, mds_pages, how, cinfo,
					    ff_layout_initiate_commit);
}