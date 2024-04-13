xfs_inode_buf_verify(
	struct xfs_buf	*bp,
	bool		readahead)
{
	struct xfs_mount *mp = bp->b_target->bt_mount;
	xfs_agnumber_t	agno;
	int		i;
	int		ni;

	/*
	 * Validate the magic number and version of every inode in the buffer
	 */
	agno = xfs_daddr_to_agno(mp, XFS_BUF_ADDR(bp));
	ni = XFS_BB_TO_FSB(mp, bp->b_length) * mp->m_sb.sb_inopblock;
	for (i = 0; i < ni; i++) {
		int		di_ok;
		xfs_dinode_t	*dip;
		xfs_agino_t	unlinked_ino;

		dip = xfs_buf_offset(bp, (i << mp->m_sb.sb_inodelog));
		unlinked_ino = be32_to_cpu(dip->di_next_unlinked);
		di_ok = dip->di_magic == cpu_to_be16(XFS_DINODE_MAGIC) &&
			xfs_dinode_good_version(mp, dip->di_version) &&
			(unlinked_ino == NULLAGINO ||
			 xfs_verify_agino(mp, agno, unlinked_ino));
		if (unlikely(XFS_TEST_ERROR(!di_ok, mp,
						XFS_ERRTAG_ITOBP_INOTOBP))) {
			if (readahead) {
				bp->b_flags &= ~XBF_DONE;
				xfs_buf_ioerror(bp, -EIO);
				return;
			}

#ifdef DEBUG
			xfs_alert(mp,
				"bad inode magic/vsn daddr %lld #%d (magic=%x)",
				(unsigned long long)bp->b_bn, i,
				be16_to_cpu(dip->di_magic));
#endif
			xfs_buf_verifier_error(bp, -EFSCORRUPTED,
					__func__, dip, sizeof(*dip),
					NULL);
			return;
		}
	}
}