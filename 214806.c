int fdt_first_region(const void *fdt,
		int (*h_include)(void *priv, const void *fdt, int offset,
				 int type, const char *data, int size),
		void *priv, struct fdt_region *region,
		char *path, int path_len, int flags,
		struct fdt_region_state *info)
{
	struct fdt_region_ptrs *p = &info->ptrs;

	/* Set up our state */
	info->fdt = fdt;
	info->can_merge = 1;
	info->max_regions = 1;
	info->start = -1;
	p->want = WANT_NOTHING;
	p->end = path;
	*p->end = '\0';
	p->nextoffset = 0;
	p->depth = -1;
	p->done = FDT_DONE_NOTHING;

	return fdt_next_region(fdt, h_include, priv, region,
			       path, path_len, flags, info);
}