GF_Err stbl_AppendSampleToChunk(GF_SampleTableBox *stbl, u32 DescIndex, u32 samplesInChunk)
{
	u32 nextChunk;
	GF_SampleToChunkBox *stsc= stbl->SampleToChunk;
	GF_StscEntry *ent;

	nextChunk = ((GF_ChunkOffsetBox *) stbl->ChunkOffset)->nb_entries;

	if (stsc->nb_entries) {
		ent = &stsc->entries[stsc->nb_entries-1];
		//good we can use this one
		if ( (ent->sampleDescriptionIndex == DescIndex) && (ent->samplesPerChunk==samplesInChunk))
			return GF_OK;

		//set the next chunk btw ...
		ent->nextChunk = nextChunk;
	}
	if (stsc->nb_entries==stsc->alloc_size) {
		ALLOC_INC(stsc->alloc_size);
		stsc->entries = gf_realloc(stsc->entries, sizeof(GF_StscEntry)*stsc->alloc_size);
		if (!stsc->entries) return GF_OUT_OF_MEM;
		memset(&stsc->entries[stsc->nb_entries], 0, sizeof(GF_StscEntry)*(stsc->alloc_size - stsc->nb_entries) );
	}
	//ok we need a new entry - this assumes this function is called AFTER AppendChunk
	ent = &stsc->entries[stsc->nb_entries];
	ent->firstChunk = nextChunk;
	ent->sampleDescriptionIndex = DescIndex;
	ent->samplesPerChunk = samplesInChunk;
	ent->isEdited = 0;
	stsc->nb_entries++;
	return GF_OK;
}