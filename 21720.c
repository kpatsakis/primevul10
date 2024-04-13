__xfs_inode_set_blocks_tag(
	xfs_inode_t	*ip,
	void		(*execute)(struct xfs_mount *mp),
	void		(*set_tp)(struct xfs_mount *mp, xfs_agnumber_t agno,
				  int error, unsigned long caller_ip),
	int		tag)
{
	struct xfs_mount *mp = ip->i_mount;
	struct xfs_perag *pag;
	int tagged;

	/*
	 * Don't bother locking the AG and looking up in the radix trees
	 * if we already know that we have the tag set.
	 */
	if (ip->i_flags & xfs_iflag_for_tag(tag))
		return;
	spin_lock(&ip->i_flags_lock);
	ip->i_flags |= xfs_iflag_for_tag(tag);
	spin_unlock(&ip->i_flags_lock);

	pag = xfs_perag_get(mp, XFS_INO_TO_AGNO(mp, ip->i_ino));
	spin_lock(&pag->pag_ici_lock);

	tagged = radix_tree_tagged(&pag->pag_ici_root, tag);
	radix_tree_tag_set(&pag->pag_ici_root,
			   XFS_INO_TO_AGINO(ip->i_mount, ip->i_ino), tag);
	if (!tagged) {
		/* propagate the eofblocks tag up into the perag radix tree */
		spin_lock(&ip->i_mount->m_perag_lock);
		radix_tree_tag_set(&ip->i_mount->m_perag_tree,
				   XFS_INO_TO_AGNO(ip->i_mount, ip->i_ino),
				   tag);
		spin_unlock(&ip->i_mount->m_perag_lock);

		/* kick off background trimming */
		execute(ip->i_mount);

		set_tp(ip->i_mount, pag->pag_agno, -1, _RET_IP_);
	}

	spin_unlock(&pag->pag_ici_lock);
	xfs_perag_put(pag);
}