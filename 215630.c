GF_Err gf_isom_update_sample(GF_ISOFile *movie, u32 trackNumber, u32 sampleNumber, GF_ISOSample *sample, Bool data_only)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_EDIT);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	e = unpack_track(trak);
	if (e) return e;

	//block for hint tracks
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_HINT) return GF_BAD_PARAM;

	//REWRITE ANY OD STUFF
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_OD) {
		GF_ISOSample *od_sample = NULL;
		e = Media_ParseODFrame(trak->Media, sample, &od_sample);
		if (!e) e = Media_UpdateSample(trak->Media, sampleNumber, od_sample, data_only);
		if (od_sample) gf_isom_sample_del(&od_sample);
	} else {
		e = Media_UpdateSample(trak->Media, sampleNumber, sample, data_only);
	}
	if (e) return e;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	gf_isom_disable_inplace_rewrite(movie);
	return GF_OK;
}