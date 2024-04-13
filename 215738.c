static GF_Err gf_isom_add_sample_group_entry(GF_List *sampleGroups, u32 sample_number, u32 grouping_type, u32 grouping_type_parameter, u32 sampleGroupDescriptionIndex, GF_List *parent, GF_SampleTableBox *stbl)
{
	GF_SampleGroupBox *sgroup = NULL;
	u32 i, count, last_sample_in_entry;
	Bool all_samples = GF_FALSE;
	assert(sampleGroups);
	count = gf_list_count(sampleGroups);
	for (i=0; i<count; i++) {
		sgroup = (GF_SampleGroupBox*)gf_list_get(sampleGroups, i);
		if (sgroup->grouping_type==grouping_type) break;
		sgroup = NULL;
	}
	if (!sgroup) {
		sgroup = (GF_SampleGroupBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_SBGP);
		if (!sgroup) return GF_OUT_OF_MEM;
		sgroup->grouping_type = grouping_type;
		sgroup->grouping_type_parameter = grouping_type_parameter;
//		gf_list_add(sampleGroups, sgroup);
		//crude patch to align old arch and filters
		gf_list_insert(sampleGroups, sgroup, 0);
		assert(parent);
		gf_list_add(parent, sgroup);
	}
	/*used in fragments, means we are adding the last sample*/
	if (!sample_number) {
		sample_number = 1;
		if (sgroup->entry_count) {
			for (i=0; i<sgroup->entry_count; i++) {
				sample_number += sgroup->sample_entries[i].sample_count;
			}
		}
	} else if (sample_number==(u32) -1) {
		all_samples = GF_TRUE;
		sample_number = 1;
	}

	if (!sgroup->entry_count) {
		u32 idx = 0;
		sgroup->entry_count = (sample_number>1) ? 2 : 1;
		sgroup->sample_entries = (GF_SampleGroupEntry*)gf_malloc(sizeof(GF_SampleGroupEntry) * sgroup->entry_count );
		if (!sgroup->sample_entries) return GF_OUT_OF_MEM;
		if (sample_number>1) {
			sgroup->sample_entries[0].sample_count = sample_number-1;
			sgroup->sample_entries[0].group_description_index = sampleGroupDescriptionIndex ? 0 : 1;
			idx = 1;
		}
		sgroup->sample_entries[idx].sample_count = 1;
		sgroup->sample_entries[idx].group_description_index = sampleGroupDescriptionIndex;
		if (all_samples && stbl) {
			sgroup->sample_entries[idx].sample_count = stbl->SampleSize->sampleCount;
		}
		return GF_OK;
	}
	if (all_samples && stbl) {
		sgroup->entry_count = 1;
		sgroup->sample_entries[0].group_description_index = sampleGroupDescriptionIndex;
		sgroup->sample_entries[0].sample_count = stbl->SampleSize->sampleCount;
		return GF_OK;
	}
	last_sample_in_entry = 0;
	for (i=0; i<sgroup->entry_count; i++) {
		/*TODO*/
		if (last_sample_in_entry + sgroup->sample_entries[i].sample_count > sample_number) return GF_NOT_SUPPORTED;
		last_sample_in_entry += sgroup->sample_entries[i].sample_count;
	}

	if (last_sample_in_entry == sample_number) {
		if (sgroup->sample_entries[sgroup->entry_count-1].group_description_index==sampleGroupDescriptionIndex)
			return GF_OK;
		else
			return GF_NOT_SUPPORTED;
	}

	if ((sgroup->sample_entries[sgroup->entry_count-1].group_description_index==sampleGroupDescriptionIndex) && (last_sample_in_entry+1==sample_number)) {
		sgroup->sample_entries[sgroup->entry_count-1].sample_count++;
		return GF_OK;
	}
	/*last entry was an empty desc (no group associated), just add the number of samples missing until new one, then add new one*/
	if (! sgroup->sample_entries[sgroup->entry_count-1].group_description_index) {
		sgroup->sample_entries[sgroup->entry_count-1].sample_count += sample_number - 1 - last_sample_in_entry;
		sgroup->sample_entries = (GF_SampleGroupEntry*)gf_realloc(sgroup->sample_entries, sizeof(GF_SampleGroupEntry) * (sgroup->entry_count + 1) );
		sgroup->sample_entries[sgroup->entry_count].sample_count = 1;
		sgroup->sample_entries[sgroup->entry_count].group_description_index = sampleGroupDescriptionIndex;
		sgroup->entry_count++;
		return GF_OK;
	}
	/*we are adding a sample with no desc, add entry at the end*/
	if (!sampleGroupDescriptionIndex || (sample_number - 1 - last_sample_in_entry==0) ) {
		sgroup->sample_entries = (GF_SampleGroupEntry*)gf_realloc(sgroup->sample_entries, sizeof(GF_SampleGroupEntry) * (sgroup->entry_count + 1) );
		sgroup->sample_entries[sgroup->entry_count].sample_count = 1;
		sgroup->sample_entries[sgroup->entry_count].group_description_index = sampleGroupDescriptionIndex;
		sgroup->entry_count++;
		return GF_OK;
	}
	/*need to insert two entries ...*/
	sgroup->sample_entries = (GF_SampleGroupEntry*)gf_realloc(sgroup->sample_entries, sizeof(GF_SampleGroupEntry) * (sgroup->entry_count + 2) );

	sgroup->sample_entries[sgroup->entry_count].sample_count = sample_number - 1 - last_sample_in_entry;
	sgroup->sample_entries[sgroup->entry_count].group_description_index = 0;

	sgroup->sample_entries[sgroup->entry_count+1].sample_count = 1;
	sgroup->sample_entries[sgroup->entry_count+1].group_description_index = sampleGroupDescriptionIndex;
	sgroup->entry_count+=2;
	return GF_OK;
}