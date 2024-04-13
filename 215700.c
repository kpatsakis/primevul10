GF_Err gf_isom_set_track_enabled(GF_ISOFile *movie, u32 trackNumber, Bool enableTrack)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (enableTrack) {
		trak->Header->flags |= 1;
	} else {
		trak->Header->flags &= ~1;
	}
	return GF_OK;
}