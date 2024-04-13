GF_Err gf_isom_set_track_magic(GF_ISOFile *movie, u32 trackNumber, u64 magic)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	trak->magic = magic;
	return GF_OK;
}