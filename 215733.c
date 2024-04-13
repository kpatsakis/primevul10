GF_Err gf_isom_set_track_creation_time(GF_ISOFile *movie,u32 trackNumber, u64 ctime, u64 mtime)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	trak->Header->creationTime = ctime;
	trak->Header->modificationTime = mtime;
	return GF_OK;
}