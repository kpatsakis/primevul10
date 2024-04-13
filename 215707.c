GF_Err gf_isom_set_sample_flags(GF_ISOFile *file, u32 track, u32 sampleNumber, u32 isLeading, u32 dependsOn, u32 dependedOn, u32 redundant)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;
	return stbl_SetDependencyType(trak->Media->information->sampleTable, sampleNumber, isLeading, dependsOn, dependedOn, redundant);
}