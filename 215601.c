GF_Err gf_isom_add_sample_info(GF_ISOFile *movie, u32 track, u32 sample_number, u32 grouping_type, u32 sampleGroupDescriptionIndex, u32 grouping_type_parameter)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_List *groupList;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, track);
	if (!trak) return GF_BAD_PARAM;

	if (!trak->Media->information->sampleTable->sampleGroups)
		trak->Media->information->sampleTable->sampleGroups = gf_list_new();

	groupList = trak->Media->information->sampleTable->sampleGroups;
	return gf_isom_add_sample_group_entry(groupList, sample_number, grouping_type, grouping_type_parameter, sampleGroupDescriptionIndex, trak->Media->information->sampleTable->child_boxes, trak->Media->information->sampleTable);
}