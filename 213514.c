ff_layout_need_layoutcommit(struct pnfs_layout_segment *lseg)
{
	return !(FF_LAYOUT_LSEG(lseg)->flags & FF_FLAGS_NO_LAYOUTCOMMIT);
}