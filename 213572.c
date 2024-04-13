ff_layout_pg_get_mirror_write(struct nfs_pageio_descriptor *desc, u32 idx)
{
	return &desc->pg_mirrors[idx];
}