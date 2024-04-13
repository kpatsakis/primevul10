GF_Err gf_isom_update_sample_reference(GF_ISOFile *movie, u32 trackNumber, u32 sampleNumber, GF_ISOSample *sample, u64 data_offset)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_EDIT);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	//block for hint tracks
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_HINT) return GF_BAD_PARAM;

	if (!sampleNumber || !sample) return GF_BAD_PARAM;

	e = unpack_track(trak);
	if (e) return e;

	//OD is not allowed as a data ref
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_OD) {
		return GF_BAD_PARAM;
	}
	//OK, update it
	e = Media_UpdateSampleReference(trak->Media, sampleNumber, sample, data_offset);
	if (e) return e;

	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	return GF_OK;
}