ff_lseg_merge(struct pnfs_layout_segment *new,
		struct pnfs_layout_segment *old)
{
	u64 new_end, old_end;

	if (test_bit(NFS_LSEG_LAYOUTRETURN, &old->pls_flags))
		return false;
	if (new->pls_range.iomode != old->pls_range.iomode)
		return false;
	old_end = pnfs_calc_offset_end(old->pls_range.offset,
			old->pls_range.length);
	if (old_end < new->pls_range.offset)
		return false;
	new_end = pnfs_calc_offset_end(new->pls_range.offset,
			new->pls_range.length);
	if (new_end < old->pls_range.offset)
		return false;
	if (!ff_lseg_match_mirrors(new, old))
		return false;

	/* Mergeable: copy info from 'old' to 'new' */
	if (new_end < old_end)
		new_end = old_end;
	if (new->pls_range.offset < old->pls_range.offset)
		new->pls_range.offset = old->pls_range.offset;
	new->pls_range.length = pnfs_calc_offset_length(new->pls_range.offset,
			new_end);
	if (test_bit(NFS_LSEG_ROC, &old->pls_flags))
		set_bit(NFS_LSEG_ROC, &new->pls_flags);
	return true;
}