int fdt_next_region(const void *fdt,
		int (*h_include)(void *priv, const void *fdt, int offset,
				 int type, const char *data, int size),
		void *priv, struct fdt_region *region,
		char *path, int path_len, int flags,
		struct fdt_region_state *info)
{
	int base = fdt_off_dt_struct(fdt);
	int last_node = 0;
	const char *str;

	info->region = region;
	info->count = 0;
	if (info->ptrs.done < FDT_DONE_MEM_RSVMAP &&
	    (flags & FDT_REG_ADD_MEM_RSVMAP)) {
		/* Add the memory reserve map into its own region */
		if (fdt_add_region(info, fdt_off_mem_rsvmap(fdt),
				   fdt_off_dt_struct(fdt) -
				   fdt_off_mem_rsvmap(fdt)))
			return 0;
		info->can_merge = 0;	/* Don't allow merging with this */
		info->ptrs.done = FDT_DONE_MEM_RSVMAP;
	}

	/*
	 * Work through the tags one by one, deciding whether each needs to
	 * be included or not. We set the variable 'include' to indicate our
	 * decision. 'want' is used to track what we want to include - it
	 * allows us to pick up all the properties (and/or subnode tags) of
	 * a node.
	 */
	while (info->ptrs.done < FDT_DONE_STRUCT) {
		const struct fdt_property *prop;
		struct fdt_region_ptrs p;
		const char *name;
		int include = 0;
		int stop_at = 0;
		uint32_t tag;
		int offset;
		int val;
		int len;

		/*
		 * Make a copy of our pointers. If we make it to the end of
		 * this block then we will commit them back to info->ptrs.
		 * Otherwise we can try again from the same starting state
		 * next time we are called.
		 */
		p = info->ptrs;

		/*
		 * Find the tag, and the offset of the next one. If we need to
		 * stop including tags, then by default we stop *after*
		 * including the current tag
		 */
		offset = p.nextoffset;
		tag = fdt_next_tag(fdt, offset, &p.nextoffset);
		stop_at = p.nextoffset;

		switch (tag) {
		case FDT_PROP:
			stop_at = offset;
			prop = fdt_get_property_by_offset(fdt, offset, NULL);
			str = fdt_string(fdt, fdt32_to_cpu(prop->nameoff));
			val = h_include(priv, fdt, last_node, FDT_IS_PROP, str,
					    strlen(str) + 1);
			if (val == -1) {
				include = p.want >= WANT_NODES_AND_PROPS;
			} else {
				include = val;
				/*
				 * Make sure we include the } for this block.
				 * It might be more correct to have this done
				 * by the call to fdt_include_supernodes() in
				 * the case where it adds the node we are
				 * currently in, but this is equivalent.
				 */
				if ((flags & FDT_REG_SUPERNODES) && val &&
				    !p.want)
					p.want = WANT_NODES_ONLY;
			}

			/* Value grepping is not yet supported */
			break;

		case FDT_NOP:
			include = p.want >= WANT_NODES_AND_PROPS;
			stop_at = offset;
			break;

		case FDT_BEGIN_NODE:
			last_node = offset;
			p.depth++;
			if (p.depth == FDT_MAX_DEPTH)
				return -FDT_ERR_BADSTRUCTURE;
			name = fdt_get_name(fdt, offset, &len);
			if (p.end - path + 2 + len >= path_len)
				return -FDT_ERR_NOSPACE;

			/* Build the full path of this node */
			if (p.end != path + 1)
				*p.end++ = '/';
			strcpy(p.end, name);
			p.end += len;
			info->stack[p.depth].want = p.want;
			info->stack[p.depth].offset = offset;

			/*
			 * If we are not intending to include this node unless
			 * it matches, make sure we stop *before* its tag.
			 */
			if (p.want == WANT_NODES_ONLY ||
			    !(flags & (FDT_REG_DIRECT_SUBNODES |
				       FDT_REG_ALL_SUBNODES))) {
				stop_at = offset;
				p.want = WANT_NOTHING;
			}
			val = h_include(priv, fdt, offset, FDT_IS_NODE, path,
					p.end - path + 1);

			/* Include this if requested */
			if (val) {
				p.want = (flags & FDT_REG_ALL_SUBNODES) ?
					WANT_ALL_NODES_AND_PROPS :
					WANT_NODES_AND_PROPS;
			}

			/* If not requested, decay our 'p.want' value */
			else if (p.want) {
				if (p.want != WANT_ALL_NODES_AND_PROPS)
					p.want--;

			/* Not including this tag, so stop now */
			} else {
				stop_at = offset;
			}

			/*
			 * Decide whether to include this tag, and update our
			 * stack with the state for this node
			 */
			include = p.want;
			info->stack[p.depth].included = include;
			break;

		case FDT_END_NODE:
			include = p.want;
			if (p.depth < 0)
				return -FDT_ERR_BADSTRUCTURE;

			/*
			 * If we don't want this node, stop right away, unless
			 * we are including subnodes
			 */
			if (!p.want && !(flags & FDT_REG_DIRECT_SUBNODES))
				stop_at = offset;
			p.want = info->stack[p.depth].want;
			p.depth--;
			while (p.end > path && *--p.end != '/')
				;
			*p.end = '\0';
			break;

		case FDT_END:
			/* We always include the end tag */
			include = 1;
			p.done = FDT_DONE_STRUCT;
			break;
		}

		/* If this tag is to be included, mark it as region start */
		if (include && info->start == -1) {
			/* Include any supernodes required by this one */
			if (flags & FDT_REG_SUPERNODES) {
				if (fdt_include_supernodes(info, p.depth))
					return 0;
			}
			info->start = offset;
		}

		/*
		 * If this tag is not to be included, finish up the current
		 * region.
		 */
		if (!include && info->start != -1) {
			if (fdt_add_region(info, base + info->start,
					   stop_at - info->start))
				return 0;
			info->start = -1;
			info->can_merge = 1;
		}

		/* If we have made it this far, we can commit our pointers */
		info->ptrs = p;
	}

	/* Add a region for the END tag and a separate one for string table */
	if (info->ptrs.done < FDT_DONE_END) {
		if (info->ptrs.nextoffset != fdt_size_dt_struct(fdt))
			return -FDT_ERR_BADSTRUCTURE;

		if (fdt_add_region(info, base + info->start,
				   info->ptrs.nextoffset - info->start))
			return 0;
		info->ptrs.done++;
	}
	if (info->ptrs.done < FDT_DONE_STRINGS) {
		if (flags & FDT_REG_ADD_STRING_TAB) {
			info->can_merge = 0;
			if (fdt_off_dt_strings(fdt) <
			    base + info->ptrs.nextoffset)
				return -FDT_ERR_BADLAYOUT;
			if (fdt_add_region(info, fdt_off_dt_strings(fdt),
					   fdt_size_dt_strings(fdt)))
				return 0;
		}
		info->ptrs.done++;
	}

	return info->count > 0 ? 0 : -FDT_ERR_NOTFOUND;
}