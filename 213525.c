ff_layout_pg_set_mirror_write(struct nfs_pageio_descriptor *desc, u32 idx)
{
	u32 old = desc->pg_mirror_idx;

	desc->pg_mirror_idx = idx;
	return old;
}