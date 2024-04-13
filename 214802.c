int fdt_add_alias_regions(const void *fdt, struct fdt_region *region, int count,
			  int max_regions, struct fdt_region_state *info)
{
	int base = fdt_off_dt_struct(fdt);
	int node, node_end, offset;
	int did_alias_header;

	node = fdt_subnode_offset(fdt, 0, "aliases");
	if (node < 0)
		return -FDT_ERR_NOTFOUND;

	/*
	 * Find the next node so that we know where the /aliases node ends. We
	 * need special handling if /aliases is the last node.
	 */
	node_end = fdt_next_subnode(fdt, node);
	if (node_end == -FDT_ERR_NOTFOUND)
		/* Move back to the FDT_END_NODE tag of '/' */
		node_end = fdt_size_dt_struct(fdt) - sizeof(fdt32_t) * 2;
	else if (node_end < 0) /* other error */
		return node_end;
	node_end -= sizeof(fdt32_t);  /* Move to FDT_END_NODE tag of /aliases */

	did_alias_header = 0;
	info->region = region;
	info->count = count;
	info->can_merge = 0;
	info->max_regions = max_regions;

	for (offset = fdt_first_property_offset(fdt, node);
	     offset >= 0;
	     offset = fdt_next_property_offset(fdt, offset)) {
		const struct fdt_property *prop;
		const char *name;
		int target, next;

		prop = fdt_get_property_by_offset(fdt, offset, NULL);
		name = fdt_string(fdt, fdt32_to_cpu(prop->nameoff));
		target = fdt_path_offset(fdt, name);
		if (!region_list_contains_offset(info, fdt, target))
			continue;
		next = fdt_next_property_offset(fdt, offset);
		if (next < 0)
			next = node_end;

		if (!did_alias_header) {
			fdt_add_region(info, base + node, 12);
			did_alias_header = 1;
		}
		fdt_add_region(info, base + offset, next - offset);
	}

	/* Add the FDT_END_NODE tag */
	if (did_alias_header)
		fdt_add_region(info, base + node_end, sizeof(fdt32_t));

	return info->count < max_regions ? info->count : -FDT_ERR_NOSPACE;
}