ff_layout_choose_any_ds_for_read(struct pnfs_layout_segment *lseg,
				 u32 start_idx, u32 *best_idx)
{
	return ff_layout_choose_ds_for_read(lseg, start_idx, best_idx, false);
}