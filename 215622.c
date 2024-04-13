static GF_Err trak_add_sample(GF_ISOFile *movie, GF_TrackBox *trak, const GF_ISOSample *sample, u32 descIndex, u64 data_offset, u32 syncShadowSampleNum)
{
	Bool skip_data = GF_FALSE;
	GF_Err e;

	//faststart mode with interleaving time, cache data until we have a full chunk
	if ((movie->storageMode==GF_ISOM_STORE_FASTSTART) && movie->interleavingTime) {
		Bool flush_chunk = GF_FALSE;
		u64 stime = sample->DTS;
		stime *= movie->moov->mvhd->timeScale;
		stime /= trak->Media->mediaHeader->timeScale;

		if (stime - trak->first_dts_chunk > movie->interleavingTime)
			flush_chunk = GF_TRUE;

		if (movie->next_flush_chunk_time < stime)
			flush_chunk = GF_TRUE;

		if (trak->chunk_stsd_idx != descIndex)
			flush_chunk = GF_TRUE;

		if (trak->Media->information->sampleTable->MaxChunkSize && trak->Media->information->sampleTable->MaxChunkSize < trak->chunk_cache_size + sample->dataLength)
			flush_chunk = GF_TRUE;

		if (flush_chunk) {
			movie->next_flush_chunk_time = stime + movie->interleavingTime;
			if (trak->chunk_cache) {
				e = gf_isom_flush_chunk(trak, GF_FALSE);
				if (e) return e;
			}
			trak->nb_samples_in_cache = 0;
			trak->chunk_cache_size = 0;
			trak->first_dts_chunk = stime;
		}
		if (!trak->chunk_cache)
			trak->chunk_cache = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		gf_bs_write_data(trak->chunk_cache, sample->data, sample->dataLength);
		trak->nb_samples_in_cache += sample->nb_pack ? sample->nb_pack : 1;
		trak->chunk_cache_size += sample->dataLength;
		trak->chunk_stsd_idx = descIndex;

		skip_data = GF_TRUE;
	}

	e = Media_AddSample(trak->Media, data_offset, sample, descIndex, syncShadowSampleNum);
	if (e) return e;

	if (!skip_data && sample->dataLength) {
		e = gf_isom_datamap_add_data(trak->Media->information->dataHandler, sample->data, sample->dataLength);
		if (e) return e;
	}

	return GF_OK;
}