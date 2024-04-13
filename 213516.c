ff_layout_get_ds_for_read(struct nfs_pageio_descriptor *pgio,
			  u32 *best_idx)
{
	struct pnfs_layout_segment *lseg = pgio->pg_lseg;
	struct nfs4_pnfs_ds *ds;

	ds = ff_layout_choose_best_ds_for_read(lseg, pgio->pg_mirror_idx,
					       best_idx);
	if (ds || !pgio->pg_mirror_idx)
		return ds;
	return ff_layout_choose_best_ds_for_read(lseg, 0, best_idx);
}