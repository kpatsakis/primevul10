ff_layout_choose_best_ds_for_read(struct pnfs_layout_segment *lseg,
				  u32 start_idx, u32 *best_idx)
{
	struct nfs4_pnfs_ds *ds;

	ds = ff_layout_choose_valid_ds_for_read(lseg, start_idx, best_idx);
	if (ds)
		return ds;
	return ff_layout_choose_any_ds_for_read(lseg, start_idx, best_idx);
}