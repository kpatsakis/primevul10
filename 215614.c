GF_Err gf_isom_set_track_flags(GF_ISOFile *movie, u32 trackNumber, u32 flags, GF_ISOMTrackFlagOp op)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (op==GF_ISOM_TKFLAGS_ADD)
		trak->Header->flags |= flags;
	else if (op==GF_ISOM_TKFLAGS_REM)
		trak->Header->flags &= ~flags;
	else
		trak->Header->flags = flags;
	return GF_OK;
}