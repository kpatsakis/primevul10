GF_Err gf_isom_set_track_priority_in_group(GF_ISOFile *movie, u32 trackNumber, u32 Priority)
{
	GF_TrackBox *trak;

	if (movie->openMode != GF_ISOM_OPEN_EDIT) return GF_ISOM_INVALID_MODE;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !Priority) return GF_BAD_PARAM;

	trak->Media->information->sampleTable->trackPriority = Priority > 255 ? 255 : Priority;
	return GF_OK;
}