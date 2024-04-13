static int fdt_add_region(struct fdt_region_state *info, int offset, int size)
{
	struct fdt_region *reg;

	reg = info->region ? &info->region[info->count - 1] : NULL;
	if (info->can_merge && info->count &&
	    info->count <= info->max_regions &&
	    reg && offset <= reg->offset + reg->size) {
		reg->size = offset + size - reg->offset;
	} else if (info->count++ < info->max_regions) {
		if (reg) {
			reg++;
			reg->offset = offset;
			reg->size = size;
		}
	} else {
		return -1;
	}

	return 0;
}