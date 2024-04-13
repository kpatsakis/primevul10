GF_Err stbl_SearchSAPs(GF_SampleTableBox *stbl, u32 SampleNumber, GF_ISOSAPType *IsRAP, u32 *prevRAP, u32 *nextRAP)
{
	u32 i, j, count, count2;
	assert(prevRAP);
	assert(nextRAP);
	(*prevRAP) = 0;
	(*nextRAP) = 0;
	(*IsRAP) = RAP_NO;

	if (!stbl->sampleGroups || !stbl->sampleGroupsDescription) return GF_OK;

	count = gf_list_count(stbl->sampleGroups);
	count2 = gf_list_count(stbl->sampleGroupsDescription);
	for (i=0; i<count; i++) {
		GF_SampleGroupDescriptionBox *sgdp = NULL;
		Bool is_rap_group = 0;
		s32 roll_distance = 0;
		u32 first_sample_in_entry, last_sample_in_entry;
		GF_SampleGroupBox *sg = gf_list_get(stbl->sampleGroups, i);
		switch (sg->grouping_type) {
		case GF_ISOM_SAMPLE_GROUP_RAP:
		case GF_ISOM_SAMPLE_GROUP_SYNC:
			is_rap_group = 1;
			break;
		case GF_ISOM_SAMPLE_GROUP_ROLL:
			break;
		default:
			continue;
		}
		for (j=0; j<count2; j++) {
			sgdp = gf_list_get(stbl->sampleGroupsDescription, j);
			if (sgdp->grouping_type==sg->grouping_type) break;
			sgdp = NULL;
		}
		if (! sgdp) continue;

		first_sample_in_entry=1;
		for (j=0; j<sg->entry_count; j++) {
			u32 first_rap_in_entry, last_rap_in_entry;
			last_sample_in_entry = first_sample_in_entry + sg->sample_entries[j].sample_count - 1;

			/*samples in this entry are not RAPs, continue*/
			if (! sg->sample_entries[j].group_description_index) {
				first_sample_in_entry += sg->sample_entries[j].sample_count;
				continue;
			}
			if (!is_rap_group) {
				GF_RollRecoveryEntry *entry = gf_list_get(sgdp->group_descriptions, sg->sample_entries[j].group_description_index - 1);
				roll_distance = entry ? entry->roll_distance : 0;
			}

			/*we consider the first sample in a roll or rap group entry to be the RAP (eg, we have to decode from this sample anyway)
			except if roll_distance is strictly negative in which case we have to rewind our sample numbers from roll_distance*/
			if (roll_distance < 0) {
				if ((s32) first_sample_in_entry + roll_distance>=0) first_rap_in_entry = first_sample_in_entry + roll_distance;
				else first_rap_in_entry = 0;

				if ((s32) last_sample_in_entry + roll_distance>=0) last_rap_in_entry = last_sample_in_entry + roll_distance;
				else last_rap_in_entry = 0;
			} else {
				first_rap_in_entry = first_sample_in_entry;
				last_rap_in_entry = last_sample_in_entry;
			}

			/*store previous & next sample RAP - note that we do not store the closest previous RAP, only the first of the previous RAP group
			as RAPs are usually isolated this should not be an issue*/
			if (first_rap_in_entry <= SampleNumber) {
				*prevRAP = first_rap_in_entry;
			}
			*nextRAP = last_rap_in_entry;
			
			/*sample lies in this (rap) group, it is rap*/
			if (is_rap_group) {
				if ((first_rap_in_entry <= SampleNumber) && (SampleNumber <= last_rap_in_entry)) {
					(*IsRAP) = RAP;
					return GF_OK;
				}
			} else {
				/*prevRAP or nextRAP matches SampleNumber, sample is RAP*/
				if ((*prevRAP == SampleNumber) || (*nextRAP == SampleNumber)) {
					(*IsRAP) = RAP;
					return GF_OK;
				}
			}

			/*first sample in entry is after our target sample, abort*/
			if (first_rap_in_entry > SampleNumber) {
				break;
			}
			first_sample_in_entry += sg->sample_entries[j].sample_count;
		}
	}
	return GF_OK;
}