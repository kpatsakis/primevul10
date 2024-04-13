ff_lseg_match_mirrors(struct pnfs_layout_segment *l1,
		struct pnfs_layout_segment *l2)
{
	const struct nfs4_ff_layout_segment *fl1 = FF_LAYOUT_LSEG(l1);
	const struct nfs4_ff_layout_segment *fl2 = FF_LAYOUT_LSEG(l1);
	u32 i;

	if (fl1->mirror_array_cnt != fl2->mirror_array_cnt)
		return false;
	for (i = 0; i < fl1->mirror_array_cnt; i++) {
		if (fl1->mirror_array[i] != fl2->mirror_array[i])
			return false;
	}
	return true;
}