xfs_iread(
	xfs_mount_t	*mp,
	xfs_trans_t	*tp,
	xfs_inode_t	*ip,
	uint		iget_flags)
{
	xfs_buf_t	*bp;
	xfs_dinode_t	*dip;
	xfs_failaddr_t	fa;
	int		error;

	/*
	 * Fill in the location information in the in-core inode.
	 */
	error = xfs_imap(mp, tp, ip->i_ino, &ip->i_imap, iget_flags);
	if (error)
		return error;

	/* shortcut IO on inode allocation if possible */
	if ((iget_flags & XFS_IGET_CREATE) &&
	    xfs_sb_version_hascrc(&mp->m_sb) &&
	    !(mp->m_flags & XFS_MOUNT_IKEEP)) {
		/* initialise the on-disk inode core */
		memset(&ip->i_d, 0, sizeof(ip->i_d));
		VFS_I(ip)->i_generation = prandom_u32();
		ip->i_d.di_version = 3;
		return 0;
	}

	/*
	 * Get pointers to the on-disk inode and the buffer containing it.
	 */
	error = xfs_imap_to_bp(mp, tp, &ip->i_imap, &dip, &bp, 0, iget_flags);
	if (error)
		return error;

	/* even unallocated inodes are verified */
	fa = xfs_dinode_verify(mp, ip->i_ino, dip);
	if (fa) {
		xfs_inode_verifier_error(ip, -EFSCORRUPTED, "dinode", dip,
				sizeof(*dip), fa);
		error = -EFSCORRUPTED;
		goto out_brelse;
	}

	/*
	 * If the on-disk inode is already linked to a directory
	 * entry, copy all of the inode into the in-core inode.
	 * xfs_iformat_fork() handles copying in the inode format
	 * specific information.
	 * Otherwise, just get the truly permanent information.
	 */
	if (dip->di_mode) {
		xfs_inode_from_disk(ip, dip);
		error = xfs_iformat_fork(ip, dip);
		if (error)  {
#ifdef DEBUG
			xfs_alert(mp, "%s: xfs_iformat() returned error %d",
				__func__, error);
#endif /* DEBUG */
			goto out_brelse;
		}
	} else {
		/*
		 * Partial initialisation of the in-core inode. Just the bits
		 * that xfs_ialloc won't overwrite or relies on being correct.
		 */
		ip->i_d.di_version = dip->di_version;
		VFS_I(ip)->i_generation = be32_to_cpu(dip->di_gen);
		ip->i_d.di_flushiter = be16_to_cpu(dip->di_flushiter);

		/*
		 * Make sure to pull in the mode here as well in
		 * case the inode is released without being used.
		 * This ensures that xfs_inactive() will see that
		 * the inode is already free and not try to mess
		 * with the uninitialized part of it.
		 */
		VFS_I(ip)->i_mode = 0;
	}

	ASSERT(ip->i_d.di_version >= 2);
	ip->i_delayed_blks = 0;

	/*
	 * Mark the buffer containing the inode as something to keep
	 * around for a while.  This helps to keep recently accessed
	 * meta-data in-core longer.
	 */
	xfs_buf_set_ref(bp, XFS_INO_REF);

	/*
	 * Use xfs_trans_brelse() to release the buffer containing the on-disk
	 * inode, because it was acquired with xfs_trans_read_buf() in
	 * xfs_imap_to_bp() above.  If tp is NULL, this is just a normal
	 * brelse().  If we're within a transaction, then xfs_trans_brelse()
	 * will only release the buffer if it is not dirty within the
	 * transaction.  It will be OK to release the buffer in this case,
	 * because inodes on disk are never destroyed and we will be locking the
	 * new in-core inode before putting it in the cache where other
	 * processes can find it.  Thus we don't have to worry about the inode
	 * being changed just because we released the buffer.
	 */
 out_brelse:
	xfs_trans_brelse(tp, bp);
	return error;
}