ff_layout_free_iostats_array(struct nfs42_layoutstat_devinfo *devinfo,
		unsigned int num_entries)
{
	unsigned int i;

	for (i = 0; i < num_entries; i++) {
		if (!devinfo[i].ld_private.ops)
			continue;
		if (!devinfo[i].ld_private.ops->free)
			continue;
		devinfo[i].ld_private.ops->free(&devinfo[i].ld_private);
	}
}