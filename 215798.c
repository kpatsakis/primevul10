GF_Err stbl_RemoveChunk(GF_SampleTableBox *stbl, u32 sampleNumber, u32 nb_samples)
{
	u32 i;
	GF_SampleToChunkBox *stsc = stbl->SampleToChunk;

	if ((nb_samples>1) && (sampleNumber>1))
		return GF_BAD_PARAM;
	
	//raw audio or constant sample size and dur
	if (stsc->nb_entries < stbl->SampleSize->sampleCount) {
		if (sampleNumber==stbl->SampleSize->sampleCount+1) {
			GF_StscEntry *ent = &stsc->entries[stsc->nb_entries-1];
			if (ent->samplesPerChunk)
				ent->samplesPerChunk--;
			if (!ent->samplesPerChunk) {
				stsc->nb_entries--;

				if (stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO) {
					((GF_ChunkOffsetBox *)stbl->ChunkOffset)->nb_entries --;
				} else {
					((GF_ChunkLargeOffsetBox *)stbl->ChunkOffset)->nb_entries --;
				}
				if (stsc->nb_entries) {
					ent = &stsc->entries[stsc->nb_entries-1];
					ent->nextChunk --;
				}
			}
			return GF_OK;
		}
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] removing sample in middle of track not supported for constant size and duration samples\n"));
		return GF_NOT_SUPPORTED;
	}

	//remove the entry in SampleToChunk (1 <-> 1 in edit mode)
	if (nb_samples==1) {
		memmove(&stsc->entries[sampleNumber-1], &stsc->entries[sampleNumber], sizeof(GF_StscEntry)*(stsc->nb_entries-sampleNumber));
		stsc->nb_entries--;

		//update the firstchunk info
		for (i=sampleNumber-1; i < stsc->nb_entries; i++) {
			assert(stsc->entries[i].firstChunk >= 1);
			stsc->entries[i].firstChunk -= 1;
			if (stsc->entries[i].nextChunk) {
				assert(stsc->entries[i].nextChunk >= 1);
				stsc->entries[i].nextChunk -= 1;
			}
		}
	} else {
		memmove(&stsc->entries[0], &stsc->entries[nb_samples], sizeof(GF_StscEntry)*(stsc->nb_entries-nb_samples));
		stsc->nb_entries -= nb_samples;

		//update the firstchunk info
		for (i=0; i < stsc->nb_entries; i++) {
			stsc->entries[i].firstChunk = i+1;
			stsc->entries[i].nextChunk = (stsc->nb_entries==i+1) ? 0 : i+2;
		}
	}
	memset(&stsc->entries[stsc->nb_entries], 0, sizeof(GF_StscEntry)*(stsc->alloc_size - stsc->nb_entries) );

	//update the cache
	stsc->firstSampleInCurrentChunk = 1;
	stsc->currentIndex = 0;
	stsc->currentChunk = 1;
	stsc->ghostNumber = 1;

	//realloc the chunk offset
	if (stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO) {
		GF_ChunkOffsetBox *stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
		if (!stbl->SampleSize->sampleCount) {
			gf_free(stco->offsets);
			stco->offsets = NULL;
			stco->nb_entries = 0;
			stco->alloc_size = 0;
			return GF_OK;
		}
		assert(stco->nb_entries - nb_samples == stbl->SampleSize->sampleCount);
		if (nb_samples==1) {
			memmove(&stco->offsets[sampleNumber-1], &stco->offsets[sampleNumber], sizeof(u32) * (stco->nb_entries - sampleNumber) );
		} else {
			memmove(&stco->offsets[0], &stco->offsets[nb_samples], sizeof(u32) * (stco->nb_entries - nb_samples) );
		}
		stco->nb_entries -= nb_samples;
	} else {
		GF_ChunkLargeOffsetBox *co64 = (GF_ChunkLargeOffsetBox *)stbl->ChunkOffset;
		if (!stbl->SampleSize->sampleCount) {
			gf_free(co64->offsets);
			co64->offsets = NULL;
			co64->nb_entries = 0;
			co64->alloc_size = 0;
			return GF_OK;
		}

		assert(co64->nb_entries - nb_samples == stbl->SampleSize->sampleCount);
		if (nb_samples==1) {
			memmove(&co64->offsets[sampleNumber-1], &co64->offsets[sampleNumber], sizeof(u64) * (co64->nb_entries - sampleNumber) );
		} else {
			memmove(&co64->offsets[0], &co64->offsets[nb_samples], sizeof(u64) * (co64->nb_entries - nb_samples) );
		}
		co64->nb_entries -= nb_samples;
	}
	return GF_OK;
}