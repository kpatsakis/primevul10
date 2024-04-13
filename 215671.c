GF_Err gf_isom_copy_sample_info(GF_ISOFile *dst, u32 dst_track, GF_ISOFile *src, u32 src_track, u32 sampleNumber)
{
	u32 i, count, idx, dst_sample_num, subs_flags;
	GF_SubSampleInfoEntry *sub_sample;
	GF_Err e;
	GF_TrackBox *src_trak, *dst_trak;

	src_trak = gf_isom_get_track_from_file(src, src_track);
	if (!src_trak) return GF_BAD_PARAM;

	dst_trak = gf_isom_get_track_from_file(dst, dst_track);
	if (!dst_trak) return GF_BAD_PARAM;

	dst_sample_num = dst_trak->Media->information->sampleTable->SampleSize->sampleCount;

	/*modify depends flags*/
	if (src_trak->Media->information->sampleTable->SampleDep) {
		u32 isLeading, dependsOn, dependedOn, redundant;

		isLeading = dependsOn = dependedOn = redundant = 0;

		e = stbl_GetSampleDepType(src_trak->Media->information->sampleTable->SampleDep, sampleNumber, &isLeading, &dependsOn, &dependedOn, &redundant);
		if (e) return e;

		e = stbl_AppendDependencyType(dst_trak->Media->information->sampleTable, isLeading, dependsOn, dependedOn, redundant);
		if (e) return e;
	}

	/*copy subsample info if any*/
	idx=1;
	while (gf_isom_get_subsample_types(src, src_track, idx, &subs_flags)) {
		GF_SubSampleInformationBox *dst_subs=NULL;
		idx++;

		if ( ! gf_isom_sample_get_subsample_entry(src, src_track, sampleNumber, subs_flags, &sub_sample))
			continue;

		/*create subsample if needed*/
		if (!dst_trak->Media->information->sampleTable->sub_samples) {
			dst_trak->Media->information->sampleTable->sub_samples = gf_list_new();
		}
		count = gf_list_count(dst_trak->Media->information->sampleTable->sub_samples);
		for (i=0; i<count; i++) {
			dst_subs = gf_list_get(dst_trak->Media->information->sampleTable->sub_samples, i);
			if (dst_subs->flags==subs_flags) break;
			dst_subs=NULL;
		}
		if (!dst_subs) {
			dst_subs = (GF_SubSampleInformationBox *) gf_isom_box_new_parent(&dst_trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_SUBS);
			if (!dst_subs) return GF_OUT_OF_MEM;
			dst_subs->version=0;
			dst_subs->flags = subs_flags;
			gf_list_add(dst_trak->Media->information->sampleTable->sub_samples, dst_subs);
		}

		count = gf_list_count(sub_sample->SubSamples);
		for (i=0; i<count; i++) {
			GF_SubSampleEntry *entry = (GF_SubSampleEntry*)gf_list_get(sub_sample->SubSamples, i);
			e = gf_isom_add_subsample_info(dst_subs, dst_sample_num, entry->subsample_size, entry->subsample_priority, entry->reserved, entry->discardable);
			if (e) return e;
		}
	}

	/*copy sampleToGroup info if any*/
	count = 0;
	if (src_trak->Media->information->sampleTable->sampleGroups)
		count = gf_list_count(src_trak->Media->information->sampleTable->sampleGroups);

	for (i=0; i<count; i++) {
		GF_SampleGroupBox *sg;
		u32 j, k, default_index;
		u32 first_sample_in_entry, last_sample_in_entry, group_desc_index_src, group_desc_index_dst;
		first_sample_in_entry = 1;

		sg = (GF_SampleGroupBox*)gf_list_get(src_trak->Media->information->sampleTable->sampleGroups, i);
		for (j=0; j<sg->entry_count; j++) {
			last_sample_in_entry = first_sample_in_entry + sg->sample_entries[j].sample_count - 1;
			if ((sampleNumber<first_sample_in_entry) || (sampleNumber>last_sample_in_entry)) {
				first_sample_in_entry = last_sample_in_entry+1;
				continue;
			}

			if (!dst_trak->Media->information->sampleTable->sampleGroups)
				dst_trak->Media->information->sampleTable->sampleGroups = gf_list_new();

			group_desc_index_src = group_desc_index_dst = sg->sample_entries[j].group_description_index;

			if (group_desc_index_src) {
				GF_SampleGroupDescriptionBox *sgd_src, *sgd_dst;
				GF_DefaultSampleGroupDescriptionEntry *sgde_src, *sgde_dst;

				group_desc_index_dst = 0;
				//check that the sample group description exists !!
				sgde_src = gf_isom_get_sample_group_info_entry(src, src_trak, sg->grouping_type, sg->sample_entries[j].group_description_index, &default_index, &sgd_src);

				if (!sgde_src) break;

				if (!dst_trak->Media->information->sampleTable->sampleGroupsDescription)
					dst_trak->Media->information->sampleTable->sampleGroupsDescription = gf_list_new();

				sgd_dst = NULL;
				for (k=0; k< gf_list_count(dst_trak->Media->information->sampleTable->sampleGroupsDescription); k++) {
					sgd_dst = gf_list_get(dst_trak->Media->information->sampleTable->sampleGroupsDescription, k);
					if (sgd_dst->grouping_type==sgd_src->grouping_type) break;
					sgd_dst = NULL;
				}
				if (!sgd_dst) {
					gf_isom_clone_box( (GF_Box *) sgd_src, (GF_Box **) &sgd_dst);
					if (!sgd_dst) return GF_OUT_OF_MEM;
					gf_list_add(dst_trak->Media->information->sampleTable->sampleGroupsDescription, sgd_dst);
				}

				//find the same entry
				for (k=0; k<gf_list_count(sgd_dst->group_descriptions); k++) {
					sgde_dst = gf_list_get(sgd_dst->group_descriptions, i);
					if (gf_isom_is_identical_sgpd(sgde_src, sgde_dst, sgd_src->grouping_type)) {
						group_desc_index_dst = k+1;
						break;
					}
				}
				if (!group_desc_index_dst) {
					GF_SampleGroupDescriptionBox *cloned=NULL;
					gf_isom_clone_box( (GF_Box *) sgd_src, (GF_Box **)  &cloned);
					if (!cloned) return GF_OUT_OF_MEM;
					sgde_dst = gf_list_get(cloned->group_descriptions, group_desc_index_dst);
					gf_list_rem(cloned->group_descriptions, group_desc_index_dst);
					gf_isom_box_del( (GF_Box *) cloned);
					gf_list_add(sgd_dst->group_descriptions, sgde_dst);
					group_desc_index_dst = gf_list_count(sgd_dst->group_descriptions);
				}
			}


			/*found our sample, add it to trak->sampleGroups*/
			e = gf_isom_add_sample_group_entry(dst_trak->Media->information->sampleTable->sampleGroups, dst_sample_num, sg->grouping_type, sg->grouping_type_parameter, group_desc_index_dst, dst_trak->Media->information->sampleTable->child_boxes, NULL);
			if (e) return e;
			break;
		}
	}

	return GF_OK;
}