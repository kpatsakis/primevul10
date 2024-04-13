GF_Err gf_isom_remove_sample_group(GF_ISOFile *movie, u32 track, u32 grouping_type)
{
	GF_Err e;
	GF_TrackBox *trak;
	u32 count, i;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, track);
	if (!trak) return GF_BAD_PARAM;

	if (trak->Media->information->sampleTable->sampleGroupsDescription) {
		count = gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription);
		for (i=0; i<count; i++) {
			GF_SampleGroupDescriptionBox *sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, i);
			if (sgdesc->grouping_type==grouping_type) {
				gf_isom_box_del_parent(&trak->Media->information->sampleTable->child_boxes, (GF_Box*)sgdesc);
				gf_list_rem(trak->Media->information->sampleTable->sampleGroupsDescription, i);
				i--;
				count--;
			}
		}
	}
	if (trak->Media->information->sampleTable->sampleGroups) {
		count = gf_list_count(trak->Media->information->sampleTable->sampleGroups);
		for (i=0; i<count; i++) {
			GF_SampleGroupBox *sgroup = gf_list_get(trak->Media->information->sampleTable->sampleGroups, i);
			if (sgroup->grouping_type==grouping_type) {
				gf_isom_box_del_parent(&trak->Media->information->sampleTable->child_boxes, (GF_Box*)sgroup);
				gf_list_rem(trak->Media->information->sampleTable->sampleGroups, i);
				i--;
				count--;
			}
		}
	}
	return GF_OK;
}