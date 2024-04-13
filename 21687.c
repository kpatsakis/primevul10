xfs_inode_clear_reclaim_tag(
	struct xfs_perag	*pag,
	xfs_ino_t		ino)
{
	radix_tree_tag_clear(&pag->pag_ici_root,
			     XFS_INO_TO_AGINO(pag->pag_mount, ino),
			     XFS_ICI_RECLAIM_TAG);
	xfs_perag_clear_reclaim_tag(pag);
}