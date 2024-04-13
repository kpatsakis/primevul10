GF_Err gf_isom_set_media_type(GF_ISOFile *movie, u32 trackNumber, u32 new_type)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !new_type) return GF_BAD_PARAM;
	trak->Media->handler->handlerType = new_type;
	return GF_OK;
}