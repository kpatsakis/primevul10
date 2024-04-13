ff_layout_free_lseg(struct pnfs_layout_segment *lseg)
{
	struct nfs4_ff_layout_segment *fls = FF_LAYOUT_LSEG(lseg);

	dprintk("--> %s\n", __func__);

	if (lseg->pls_range.iomode == IOMODE_RW) {
		struct nfs4_flexfile_layout *ffl;
		struct inode *inode;

		ffl = FF_LAYOUT_FROM_HDR(lseg->pls_layout);
		inode = ffl->generic_hdr.plh_inode;
		spin_lock(&inode->i_lock);
		pnfs_generic_ds_cinfo_release_lseg(&ffl->commit_info, lseg);
		spin_unlock(&inode->i_lock);
	}
	_ff_layout_free_lseg(fls);
}