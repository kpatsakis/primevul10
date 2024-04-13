GF_EXPORT
GF_Err gf_isom_get_chunk_info(GF_ISOFile *movie, u32 trackNumber, u32 chunk_num, u64 *chunk_offset, u32 *first_sample_num, u32 *sample_per_chunk, u32 *sample_desc_idx)
{
	GF_ChunkOffsetBox *stco = NULL;
	GF_ChunkLargeOffsetBox *co64 = NULL;
	GF_SampleToChunkBox *stsc = NULL;
	GF_TrackBox *trak;
	u32 i, nb_entries, nb_samples, sample_desc_index;
	if (!movie || !movie->moov || !trackNumber || !chunk_num) return GF_BAD_PARAM;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->ChunkOffset ) return GF_BAD_PARAM;

	stsc = (GF_SampleToChunkBox *) trak->Media->information->sampleTable->SampleToChunk;
	stco = (GF_ChunkOffsetBox *) trak->Media->information->sampleTable->ChunkOffset;
	if (stco->type == GF_ISOM_BOX_TYPE_CO64) {
		stco = NULL;
		co64 = (GF_ChunkLargeOffsetBox *) trak->Media->information->sampleTable->ChunkOffset;
		nb_entries = co64->nb_entries;
	} else {
		nb_entries = stco->nb_entries;
	}
	if (chunk_num>nb_entries) return GF_BAD_PARAM;

	sample_desc_index = 0;
	nb_samples = 1;
	for (i=0; i<stsc->nb_entries; i++) {
		u32 nb_chunks_before;

		if (stsc->entries[i].firstChunk == chunk_num) {
			sample_desc_index = stsc->entries[i].sampleDescriptionIndex;
			if (sample_per_chunk)
				*sample_per_chunk = stsc->entries[i].samplesPerChunk;
			break;
		}
		assert(stsc->entries[i].firstChunk<chunk_num);

		if ((i+1 == stsc->nb_entries)
			|| (stsc->entries[i+1].firstChunk>chunk_num)
		) {
			nb_chunks_before = chunk_num - stsc->entries[i].firstChunk;
			nb_samples += stsc->entries[i].samplesPerChunk * nb_chunks_before;
			sample_desc_index = stsc->entries[i].sampleDescriptionIndex;
			if (sample_per_chunk)
				*sample_per_chunk = stsc->entries[i].samplesPerChunk;
			break;
		}
		assert(stsc->entries[i+1].firstChunk > stsc->entries[i].firstChunk);

		nb_chunks_before = stsc->entries[i+1].firstChunk - stsc->entries[i].firstChunk;
		nb_samples += stsc->entries[i].samplesPerChunk * nb_chunks_before;
	}
	if (first_sample_num) *first_sample_num = nb_samples;
	if (sample_desc_idx) *sample_desc_idx = sample_desc_index;
	if (chunk_offset) {
		if (stco)
			*chunk_offset = stco->offsets[chunk_num-1];
		else
			*chunk_offset = co64->offsets[chunk_num-1];
	}
	return GF_OK;