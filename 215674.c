GF_Err gf_isom_set_media_creation_time(GF_ISOFile *movie,u32 trackNumber, u64 ctime, u64 mtime)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media || !trak->Media->mediaHeader) return GF_ISOM_INVALID_FILE;

	trak->Media->mediaHeader->creationTime = ctime;
	trak->Media->mediaHeader->modificationTime = mtime;
	return GF_OK;
}