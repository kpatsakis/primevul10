static int fdt_include_supernodes(struct fdt_region_state *info, int depth)
{
	int base = fdt_off_dt_struct(info->fdt);
	int start, stop_at;
	int i;

	/*
	 * Work down the stack looking for supernodes that we didn't include.
	 * The algortihm here is actually pretty simple, since we know that
	 * no previous subnode had to include these nodes, or if it did, we
	 * marked them as included (on the stack) already.
	 */
	for (i = 0; i <= depth; i++) {
		if (!info->stack[i].included) {
			start = info->stack[i].offset;

			/* Add the FDT_BEGIN_NODE tag of this supernode */
			fdt_next_tag(info->fdt, start, &stop_at);
			if (fdt_add_region(info, base + start, stop_at - start))
				return -1;

			/* Remember that this supernode is now included */
			info->stack[i].included = 1;
			info->can_merge = 1;
		}

		/* Force (later) generation of the FDT_END_NODE tag */
		if (!info->stack[i].want)
			info->stack[i].want = WANT_NODES_ONLY;
	}

	return 0;
}