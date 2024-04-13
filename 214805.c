static int region_list_contains_offset(struct fdt_region_state *info,
				       const void *fdt, int target)
{
	struct fdt_region *reg;
	int num;

	target += fdt_off_dt_struct(fdt);
	for (reg = info->region, num = 0; num < info->count; reg++, num++) {
		if (target >= reg->offset && target < reg->offset + reg->size)
			return 1;
	}

	return 0;
}