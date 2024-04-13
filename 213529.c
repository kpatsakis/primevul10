ff_layout_set_layoutcommit(struct inode *inode,
		struct pnfs_layout_segment *lseg,
		loff_t end_offset)
{
	if (!ff_layout_need_layoutcommit(lseg))
		return;

	pnfs_set_layoutcommit(inode, lseg, end_offset);
	dprintk("%s inode %lu pls_end_pos %llu\n", __func__, inode->i_ino,
		(unsigned long long) NFS_I(inode)->layout->plh_lwb);
}