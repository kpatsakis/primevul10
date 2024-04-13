ff_layout_add_lseg(struct pnfs_layout_hdr *lo,
		struct pnfs_layout_segment *lseg,
		struct list_head *free_me)
{
	pnfs_generic_layout_insert_lseg(lo, lseg,
			ff_lseg_range_is_after,
			ff_lseg_merge,
			free_me);
}