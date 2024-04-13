ff_layout_add_mirror(struct pnfs_layout_hdr *lo,
		struct nfs4_ff_layout_mirror *mirror)
{
	struct nfs4_flexfile_layout *ff_layout = FF_LAYOUT_FROM_HDR(lo);
	struct nfs4_ff_layout_mirror *pos;
	struct inode *inode = lo->plh_inode;

	spin_lock(&inode->i_lock);
	list_for_each_entry(pos, &ff_layout->mirrors, mirrors) {
		if (memcmp(&mirror->devid, &pos->devid, sizeof(pos->devid)) != 0)
			continue;
		if (!ff_mirror_match_fh(mirror, pos))
			continue;
		if (refcount_inc_not_zero(&pos->ref)) {
			spin_unlock(&inode->i_lock);
			return pos;
		}
	}
	list_add(&mirror->mirrors, &ff_layout->mirrors);
	mirror->layout = lo;
	spin_unlock(&inode->i_lock);
	return mirror;
}