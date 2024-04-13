__xfs_inode_clear_blocks_tag(
	xfs_inode_t	*ip,
	void		(*clear_tp)(struct xfs_mount *mp, xfs_agnumber_t agno,
				    int error, unsigned long caller_ip),
	int		tag)
{
	struct xfs_mount *mp = ip->i_mount;
	struct xfs_perag *pag;

	spin_lock(&ip->i_flags_lock);
	ip->i_flags &= ~xfs_iflag_for_tag(tag);
	spin_unlock(&ip->i_flags_lock);

	pag = xfs_perag_get(mp, XFS_INO_TO_AGNO(mp, ip->i_ino));
	spin_lock(&pag->pag_ici_lock);

	radix_tree_tag_clear(&pag->pag_ici_root,
			     XFS_INO_TO_AGINO(ip->i_mount, ip->i_ino), tag);
	if (!radix_tree_tagged(&pag->pag_ici_root, tag)) {
		/* clear the eofblocks tag from the perag radix tree */
		spin_lock(&ip->i_mount->m_perag_lock);
		radix_tree_tag_clear(&ip->i_mount->m_perag_tree,
				     XFS_INO_TO_AGNO(ip->i_mount, ip->i_ino),
				     tag);
		spin_unlock(&ip->i_mount->m_perag_lock);
		clear_tp(ip->i_mount, pag->pag_agno, -1, _RET_IP_);
	}

	spin_unlock(&pag->pag_ici_lock);
	xfs_perag_put(pag);
}