GF_Err gf_isom_flush_chunk(GF_TrackBox *trak, Bool is_final)
{
	GF_Err e;
	u64 data_offset;
	u32 sample_number;
	u8 *chunk_data;
	u32 chunk_size, chunk_alloc;
	if (!trak->chunk_cache) return GF_OK;

	gf_bs_get_content_no_truncate(trak->chunk_cache, &chunk_data, &chunk_size, &chunk_alloc);

	data_offset = gf_isom_datamap_get_offset(trak->Media->information->dataHandler);

	e = gf_isom_datamap_add_data(trak->Media->information->dataHandler, chunk_data, chunk_size);
	if (e) return e;

	sample_number = 1 + trak->Media->information->sampleTable->SampleSize->sampleCount;
	sample_number -= trak->nb_samples_in_cache;

	e = stbl_AddChunkOffset(trak->Media, sample_number, trak->chunk_stsd_idx, data_offset, trak->nb_samples_in_cache);

	if (is_final) {
		gf_free(chunk_data);
		gf_bs_del(trak->chunk_cache);
		trak->chunk_cache = NULL;
	} else {
		gf_bs_reassign_buffer(trak->chunk_cache, chunk_data, chunk_alloc);
	}
	return e;
}