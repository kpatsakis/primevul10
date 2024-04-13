ff_layout_choose_ds_for_read(struct pnfs_layout_segment *lseg,
			     u32 start_idx, u32 *best_idx,
			     bool check_device)
{
	struct nfs4_ff_layout_segment *fls = FF_LAYOUT_LSEG(lseg);
	struct nfs4_ff_layout_mirror *mirror;
	struct nfs4_pnfs_ds *ds;
	u32 idx;

	/* mirrors are initially sorted by efficiency */
	for (idx = start_idx; idx < fls->mirror_array_cnt; idx++) {
		mirror = FF_LAYOUT_COMP(lseg, idx);
		ds = nfs4_ff_layout_prepare_ds(lseg, mirror, false);
		if (!ds)
			continue;

		if (check_device &&
		    nfs4_test_deviceid_unavailable(&mirror->mirror_ds->id_node))
			continue;

		*best_idx = idx;
		return ds;
	}

	return NULL;
}