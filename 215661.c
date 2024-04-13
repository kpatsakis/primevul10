static GF_SampleGroupDescriptionBox *get_sgdp(GF_SampleTableBox *stbl, void *traf, u32 grouping_type, Bool *is_traf_sgdp)
#endif /* GPAC_DISABLE_ISOM_FRAGMENTS */
{
	GF_List *groupList=NULL;
	GF_List **parent=NULL;
	GF_SampleGroupDescriptionBox *sgdesc=NULL;
	u32 count, i;

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (!stbl && traf && traf->trex->track->Media->information->sampleTable)
		stbl = traf->trex->track->Media->information->sampleTable;
#endif
	if (stbl) {
		if (!stbl->sampleGroupsDescription && !traf)
			stbl->sampleGroupsDescription = gf_list_new();

		groupList = stbl->sampleGroupsDescription;
		if (is_traf_sgdp) *is_traf_sgdp = GF_FALSE;
		parent = &stbl->child_boxes;

		count = gf_list_count(groupList);
		for (i=0; i<count; i++) {
			sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(groupList, i);
			if (sgdesc->grouping_type==grouping_type) break;
			sgdesc = NULL;
		}
	}
	
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	/*look in stbl or traf for sample sampleGroupsDescription*/
	if (!sgdesc && traf) {
		if (!traf->sampleGroupsDescription)
			traf->sampleGroupsDescription = gf_list_new();
		groupList = traf->sampleGroupsDescription;
		parent = &traf->child_boxes;
		if (is_traf_sgdp) *is_traf_sgdp = GF_TRUE;

		count = gf_list_count(groupList);
		for (i=0; i<count; i++) {
			sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(groupList, i);
			if (sgdesc->grouping_type==grouping_type) break;
			sgdesc = NULL;
		}
	}
#endif

	if (!sgdesc) {
		sgdesc = (GF_SampleGroupDescriptionBox *) gf_isom_box_new_parent(parent, GF_ISOM_BOX_TYPE_SGPD);
		if (!sgdesc) return NULL;
		sgdesc->grouping_type = grouping_type;
		assert(groupList);
		gf_list_add(groupList, sgdesc);
	}
	return sgdesc;
}