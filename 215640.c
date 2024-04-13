GF_Err gf_isom_set_alternate_group_id(GF_ISOFile *movie, u32 trackNumber, u32 groupId)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	trak->Header->alternate_group = groupId;
	return GF_OK;
}