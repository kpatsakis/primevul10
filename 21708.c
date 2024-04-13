xfs_reclaim_inode(
	struct xfs_inode	*ip,
	struct xfs_perag	*pag,
	int			sync_mode)
{
	struct xfs_buf		*bp = NULL;
	xfs_ino_t		ino = ip->i_ino; /* for radix_tree_delete */
	int			error;

restart:
	error = 0;
	xfs_ilock(ip, XFS_ILOCK_EXCL);
	if (!xfs_iflock_nowait(ip)) {
		if (!(sync_mode & SYNC_WAIT))
			goto out;
		xfs_iflock(ip);
	}

	if (XFS_FORCED_SHUTDOWN(ip->i_mount)) {
		xfs_iunpin_wait(ip);
		/* xfs_iflush_abort() drops the flush lock */
		xfs_iflush_abort(ip, false);
		goto reclaim;
	}
	if (xfs_ipincount(ip)) {
		if (!(sync_mode & SYNC_WAIT))
			goto out_ifunlock;
		xfs_iunpin_wait(ip);
	}
	if (xfs_iflags_test(ip, XFS_ISTALE) || xfs_inode_clean(ip)) {
		xfs_ifunlock(ip);
		goto reclaim;
	}

	/*
	 * Never flush out dirty data during non-blocking reclaim, as it would
	 * just contend with AIL pushing trying to do the same job.
	 */
	if (!(sync_mode & SYNC_WAIT))
		goto out_ifunlock;

	/*
	 * Now we have an inode that needs flushing.
	 *
	 * Note that xfs_iflush will never block on the inode buffer lock, as
	 * xfs_ifree_cluster() can lock the inode buffer before it locks the
	 * ip->i_lock, and we are doing the exact opposite here.  As a result,
	 * doing a blocking xfs_imap_to_bp() to get the cluster buffer would
	 * result in an ABBA deadlock with xfs_ifree_cluster().
	 *
	 * As xfs_ifree_cluser() must gather all inodes that are active in the
	 * cache to mark them stale, if we hit this case we don't actually want
	 * to do IO here - we want the inode marked stale so we can simply
	 * reclaim it.  Hence if we get an EAGAIN error here,  just unlock the
	 * inode, back off and try again.  Hopefully the next pass through will
	 * see the stale flag set on the inode.
	 */
	error = xfs_iflush(ip, &bp);
	if (error == -EAGAIN) {
		xfs_iunlock(ip, XFS_ILOCK_EXCL);
		/* backoff longer than in xfs_ifree_cluster */
		delay(2);
		goto restart;
	}

	if (!error) {
		error = xfs_bwrite(bp);
		xfs_buf_relse(bp);
	}

reclaim:
	ASSERT(!xfs_isiflocked(ip));

	/*
	 * Because we use RCU freeing we need to ensure the inode always appears
	 * to be reclaimed with an invalid inode number when in the free state.
	 * We do this as early as possible under the ILOCK so that
	 * xfs_iflush_cluster() and xfs_ifree_cluster() can be guaranteed to
	 * detect races with us here. By doing this, we guarantee that once
	 * xfs_iflush_cluster() or xfs_ifree_cluster() has locked XFS_ILOCK that
	 * it will see either a valid inode that will serialise correctly, or it
	 * will see an invalid inode that it can skip.
	 */
	spin_lock(&ip->i_flags_lock);
	ip->i_flags = XFS_IRECLAIM;
	ip->i_ino = 0;
	spin_unlock(&ip->i_flags_lock);

	xfs_iunlock(ip, XFS_ILOCK_EXCL);

	XFS_STATS_INC(ip->i_mount, xs_ig_reclaims);
	/*
	 * Remove the inode from the per-AG radix tree.
	 *
	 * Because radix_tree_delete won't complain even if the item was never
	 * added to the tree assert that it's been there before to catch
	 * problems with the inode life time early on.
	 */
	spin_lock(&pag->pag_ici_lock);
	if (!radix_tree_delete(&pag->pag_ici_root,
				XFS_INO_TO_AGINO(ip->i_mount, ino)))
		ASSERT(0);
	xfs_perag_clear_reclaim_tag(pag);
	spin_unlock(&pag->pag_ici_lock);

	/*
	 * Here we do an (almost) spurious inode lock in order to coordinate
	 * with inode cache radix tree lookups.  This is because the lookup
	 * can reference the inodes in the cache without taking references.
	 *
	 * We make that OK here by ensuring that we wait until the inode is
	 * unlocked after the lookup before we go ahead and free it.
	 */
	xfs_ilock(ip, XFS_ILOCK_EXCL);
	xfs_qm_dqdetach(ip);
	xfs_iunlock(ip, XFS_ILOCK_EXCL);

	__xfs_inode_free(ip);
	return error;

out_ifunlock:
	xfs_ifunlock(ip);
out:
	xfs_iflags_clear(ip, XFS_IRECLAIM);
	xfs_iunlock(ip, XFS_ILOCK_EXCL);
	/*
	 * We could return -EAGAIN here to make reclaim rescan the inode tree in
	 * a short while. However, this just burns CPU time scanning the tree
	 * waiting for IO to complete and the reclaim work never goes back to
	 * the idle state. Instead, return 0 to let the next scheduled
	 * background reclaim attempt to reclaim the inode again.
	 */
	return 0;
}