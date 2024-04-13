static u32 get_box_reg_idx(u32 boxCode, u32 parent_type)
{
	u32 i=0, count = gf_isom_get_num_supported_boxes();
	const char *parent_name = parent_type ? gf_4cc_to_str(parent_type) : NULL;

	for (i=1; i<count; i++) {
		if (box_registry[i].box_4cc==boxCode) {
			if (!parent_type) return i;
			if (strstr(box_registry[i].parents_4cc, parent_name) != NULL) return i;

			if (strstr(box_registry[i].parents_4cc, "sample_entry") != NULL) {
				u32 j = get_box_reg_idx(parent_type, 0);
				if (box_registry[j].parents_4cc && (strstr(box_registry[j].parents_4cc, "stsd") != NULL))
					return i;
			}
		}
	}
	return 0;
}