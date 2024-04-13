xfs_imap_to_bp(
	struct xfs_mount	*mp,
	struct xfs_trans	*tp,
	struct xfs_imap		*imap,
	struct xfs_dinode       **dipp,
	struct xfs_buf		**bpp,
	uint			buf_flags,
	uint			iget_flags)
{
	struct xfs_buf		*bp;
	int			error;

	buf_flags |= XBF_UNMAPPED;
	error = xfs_trans_read_buf(mp, tp, mp->m_ddev_targp, imap->im_blkno,
				   (int)imap->im_len, buf_flags, &bp,
				   &xfs_inode_buf_ops);
	if (error) {
		if (error == -EAGAIN) {
			ASSERT(buf_flags & XBF_TRYLOCK);
			return error;
		}

		if (error == -EFSCORRUPTED &&
		    (iget_flags & XFS_IGET_UNTRUSTED))
			return -EINVAL;

		xfs_warn(mp, "%s: xfs_trans_read_buf() returned error %d.",
			__func__, error);
		return error;
	}

	*bpp = bp;
	*dipp = xfs_buf_offset(bp, imap->im_boffset);
	return 0;
}