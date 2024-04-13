xfs_inode_ag_iterator_tag(
	struct xfs_mount	*mp,
	int			(*execute)(struct xfs_inode *ip, int flags,
					   void *args),
	int			flags,
	void			*args,
	int			tag)
{
	struct xfs_perag	*pag;
	int			error = 0;
	int			last_error = 0;
	xfs_agnumber_t		ag;

	ag = 0;
	while ((pag = xfs_perag_get_tag(mp, ag, tag))) {
		ag = pag->pag_agno + 1;
		error = xfs_inode_ag_walk(mp, pag, execute, flags, args, tag,
					  0);
		xfs_perag_put(pag);
		if (error) {
			last_error = error;
			if (error == -EFSCORRUPTED)
				break;
		}
	}
	return last_error;
}