GF_Err gf_isom_hint_max_chunk_size(GF_ISOFile *movie, u32 trackNumber, u32 maxChunkSize)
{
	GF_TrackBox *trak;

	if (movie->openMode == GF_ISOM_OPEN_READ) return GF_ISOM_INVALID_MODE;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !maxChunkSize) return GF_BAD_PARAM;

	trak->Media->information->sampleTable->MaxChunkSize = maxChunkSize;
	return GF_OK;
}