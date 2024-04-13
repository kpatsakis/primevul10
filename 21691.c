xfs_inode_ag_iterator(
	struct xfs_mount	*mp,
	int			(*execute)(struct xfs_inode *ip, int flags,
					   void *args),
	int			flags,
	void			*args)
{
	return xfs_inode_ag_iterator_flags(mp, execute, flags, args, 0);
}