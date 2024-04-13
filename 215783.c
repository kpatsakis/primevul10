GF_Err stbl_SetChunkAndOffset(GF_SampleTableBox *stbl, u32 sampleNumber, u32 StreamDescIndex, GF_SampleToChunkBox *the_stsc, GF_Box **the_stco, u64 data_offset, Bool forceNewChunk, u32 nb_samp)
{
	GF_Err e;
	u8 newChunk;
	GF_StscEntry *newEnt, *cur_ent;

	if (!stbl) return GF_ISOM_INVALID_FILE;

	newChunk = 0;
	//do we need a new chunk ??? For that, we need
	//1 - make sure this sample data is contiguous to the prev one

	//force new chunk is set during writing (flat / interleaved)
	//it is set to 1 when data is not contiguous in the media (eg, interleaving)
	//when writing flat files, it is never used
	if (forceNewChunk) newChunk = 1;

	cur_ent = NULL;
	//2 - make sure we have the table inited (i=0)
	if (! the_stsc->entries) {
		newChunk = 1;
	} else {
		cur_ent = &the_stsc->entries[the_stsc->nb_entries - 1];
		//3 - make sure we do not exceed the MaxSamplesPerChunk and we have the same descIndex
		if (StreamDescIndex != cur_ent->sampleDescriptionIndex)
			newChunk = 1;
		if (stbl->MaxSamplePerChunk && cur_ent->samplesPerChunk >= stbl->MaxSamplePerChunk)
			newChunk = 1;
	}

	//no need for a new chunk
	if (!newChunk) {
		cur_ent->samplesPerChunk += nb_samp;
		return GF_OK;
	}

	//OK, we have to create a new chunk...
	//check if we can remove the current sampleToChunk entry (same properties)
	if (the_stsc->nb_entries > 1) {
		GF_StscEntry *ent = &the_stsc->entries[the_stsc->nb_entries - 2];
		if (!ent) return GF_OUT_OF_MEM;
		if ( (ent->sampleDescriptionIndex == cur_ent->sampleDescriptionIndex)
		        && (ent->samplesPerChunk == cur_ent->samplesPerChunk)
		   ) {
			//OK, it's the same SampleToChunk, so delete it
			ent->nextChunk = cur_ent->firstChunk;
			the_stsc->nb_entries--;
		}
	}

	//add our offset
	e = stbl_AddOffset(stbl, the_stco, data_offset);
	if (e) return e;

	if (the_stsc->nb_entries==the_stsc->alloc_size) {
		ALLOC_INC(the_stsc->alloc_size);
		the_stsc->entries = gf_realloc(the_stsc->entries, sizeof(GF_StscEntry)*the_stsc->alloc_size);
		if (!the_stsc->entries) return GF_OUT_OF_MEM;
		memset(&the_stsc->entries[the_stsc->nb_entries], 0, sizeof(GF_StscEntry)*(the_stsc->alloc_size-the_stsc->nb_entries));
	}
	//create a new entry (could be the first one, BTW)
	newEnt = &the_stsc->entries[the_stsc->nb_entries];
	if (!newEnt) return GF_OUT_OF_MEM;

	//get the first chunk value
	if ((*the_stco)->type == GF_ISOM_BOX_TYPE_STCO) {
		newEnt->firstChunk = ((GF_ChunkOffsetBox *) (*the_stco) )->nb_entries;
	} else {
		newEnt->firstChunk = ((GF_ChunkLargeOffsetBox *) (*the_stco) )->nb_entries;
	}
	newEnt->sampleDescriptionIndex = StreamDescIndex;
	newEnt->samplesPerChunk = nb_samp;
	newEnt->nextChunk = 0;
	//if we already have an entry, adjust its next chunk to point to our new chunk
	if (the_stsc->nb_entries)
		the_stsc->entries[the_stsc->nb_entries-1].nextChunk = newEnt->firstChunk;
	the_stsc->nb_entries++;
	return GF_OK;
}