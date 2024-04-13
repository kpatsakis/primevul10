GF_Err gf_isom_sample_set_dep_info(GF_ISOFile *file, u32 track, u32 sampleNumber, u32 isLeading, u32 dependsOn, u32 dependedOn, u32 redundant)
{
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;

	return stbl_AddDependencyType(trak->Media->information->sampleTable, sampleNumber, isLeading, dependsOn, dependedOn, redundant);
}