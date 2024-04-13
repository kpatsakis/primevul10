GF_Err stbl_RemoveSampleGroup(GF_SampleTableBox *stbl, u32 SampleNumber)
{
	u32 i, k, count, prev_sample;

	if (!stbl->sampleGroups) return GF_OK;

	count = gf_list_count(stbl->sampleGroups);
	prev_sample = 0;
	for (i=0; i<count; i++) {
		GF_SampleGroupBox *e = gf_list_get(stbl->sampleGroups, i);
		for (k=0; k<e->entry_count; k++) {
			if ((SampleNumber>prev_sample) && (SampleNumber <= prev_sample + e->sample_entries[k].sample_count) ) {
				e->sample_entries[k].sample_count--;
				if (!e->sample_entries[k].sample_count) {
					memmove(&e->sample_entries[k], &e->sample_entries[k+1], sizeof(GF_SampleGroupEntry) * (e->entry_count-k-1));
					e->entry_count--;
				}
				break;
			}
		}
		if (!e->entry_count) {
			gf_list_rem(stbl->sampleGroups, i);
			i--;
			count--;
			gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) e);
		}
	}
	return GF_OK;
}