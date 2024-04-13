static GF_Err gf_isom_set_sample_group_info_ex(GF_SampleTableBox *stbl, void *traf, u32 sample_number, u32 grouping_type, u32 grouping_type_parameter, void *udta, void *(*sg_create_entry)(void *udta), Bool (*sg_compare_entry)(void *udta, void *entry))
#endif /* GPAC_DISABLE_ISOM_FRAGMENTS */
{
	GF_List *groupList, *parent;
	void *entry;
	Bool is_traf_sgpd;
	GF_SampleGroupDescriptionBox *sgdesc = NULL;
	u32 i, entry_idx;

	if (!stbl && !traf) return GF_BAD_PARAM;

	sgdesc = get_sgdp(stbl, traf, grouping_type, &is_traf_sgpd);
	if (!sgdesc) return GF_OUT_OF_MEM;

	entry = NULL;
	if (sg_compare_entry) {
		for (i=0; i<gf_list_count(sgdesc->group_descriptions); i++) {
			entry = gf_list_get(sgdesc->group_descriptions, i);
			if (sg_compare_entry(udta, entry)) break;
			entry = NULL;
		}
	}
	if (!entry && sg_create_entry) {
		entry = sg_create_entry(udta);
		if (!entry) return GF_IO_ERR;
		if (traf && !is_traf_sgpd) {
			sgdesc = get_sgdp(NULL, traf, grouping_type, &is_traf_sgpd);
		}
		gf_list_add(sgdesc->group_descriptions, entry);
	}
	if (!entry)
		entry_idx = 0;
	else
		entry_idx = 1 + gf_list_find(sgdesc->group_descriptions, entry);

	/*look in stbl or traf for sample sampleGroups*/
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (traf) {
		if (!traf->sampleGroups)
			traf->sampleGroups = gf_list_new();
		groupList = traf->sampleGroups;
		parent = traf->child_boxes;
		if (entry_idx && is_traf_sgpd)
			entry_idx |= 0x10000;
	} else
#endif
	{
		if (!stbl->sampleGroups)
			stbl->sampleGroups = gf_list_new();
		groupList = stbl->sampleGroups;
		parent = stbl->child_boxes;
	}

	return gf_isom_add_sample_group_entry(groupList, sample_number, grouping_type, grouping_type_parameter, entry_idx, parent, stbl);
}