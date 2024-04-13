xfs_inode_free_callback(
	struct rcu_head		*head)
{
	struct inode		*inode = container_of(head, struct inode, i_rcu);
	struct xfs_inode	*ip = XFS_I(inode);

	switch (VFS_I(ip)->i_mode & S_IFMT) {
	case S_IFREG:
	case S_IFDIR:
	case S_IFLNK:
		xfs_idestroy_fork(ip, XFS_DATA_FORK);
		break;
	}

	if (ip->i_afp)
		xfs_idestroy_fork(ip, XFS_ATTR_FORK);
	if (ip->i_cowfp)
		xfs_idestroy_fork(ip, XFS_COW_FORK);

	if (ip->i_itemp) {
		ASSERT(!(ip->i_itemp->ili_item.li_flags & XFS_LI_IN_AIL));
		xfs_inode_item_destroy(ip);
		ip->i_itemp = NULL;
	}

	kmem_zone_free(xfs_inode_zone, ip);
}