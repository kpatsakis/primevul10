GF_Err stbl_UnpackOffsets(GF_SampleTableBox *stbl)
{
	GF_Err e;
	u32 i, chunkNumber, sampleDescIndex;
	u64 dataOffset;
	GF_StscEntry *ent;
	GF_ChunkOffsetBox *stco_tmp;
	GF_ChunkLargeOffsetBox *co64_tmp;
	GF_SampleToChunkBox *stsc_tmp;

	if (!stbl) return GF_ISOM_INVALID_FILE;

	//we should have none of the mandatory boxes (allowed in the spec)
	if (!stbl->ChunkOffset && !stbl->SampleDescription && !stbl->SampleSize && !stbl->SampleToChunk && !stbl->TimeToSample)
		return GF_OK;
	/*empty track (just created)*/
	if (!stbl->SampleToChunk && !stbl->TimeToSample) return GF_OK;

	//or all the mandatory ones ...
	if (!stbl->ChunkOffset || !stbl->SampleDescription || !stbl->SampleSize || !stbl->SampleToChunk || !stbl->TimeToSample)
		return GF_ISOM_INVALID_FILE;

	//do we need to unpack? Not if we have only one sample per chunk.
	if (stbl->SampleSize->sampleCount == stbl->SampleToChunk->nb_entries) return GF_OK;

	//check the offset type and create a new table...
	if (stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO) {
		co64_tmp = NULL;
		stco_tmp = (GF_ChunkOffsetBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STCO);
		if (!stco_tmp) return GF_OUT_OF_MEM;
		stco_tmp->nb_entries = stbl->SampleSize->sampleCount;
		stco_tmp->offsets = (u32*)gf_malloc(stco_tmp->nb_entries * sizeof(u32));
		if (!stco_tmp->offsets) {
			gf_isom_box_del((GF_Box*)stco_tmp);
			return GF_OUT_OF_MEM;
		}
		stco_tmp->alloc_size = stco_tmp->nb_entries;
	} else if (stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_CO64) {
		stco_tmp = NULL;
		co64_tmp = (GF_ChunkLargeOffsetBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_CO64);
		if (!co64_tmp) return GF_OUT_OF_MEM;
		co64_tmp->nb_entries = stbl->SampleSize->sampleCount;
		co64_tmp->offsets = (u64*)gf_malloc(co64_tmp->nb_entries * sizeof(u64));
		if (!co64_tmp->offsets) {
			gf_isom_box_del((GF_Box*)co64_tmp);
			return GF_OUT_OF_MEM;
		}
		co64_tmp->alloc_size = co64_tmp->nb_entries;
	} else {
		return GF_ISOM_INVALID_FILE;
	}

	//create a new SampleToChunk table
	stsc_tmp = (GF_SampleToChunkBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STSC);
	if (!stsc_tmp) return GF_OUT_OF_MEM;

	stsc_tmp->nb_entries = stsc_tmp->alloc_size = stbl->SampleSize->sampleCount;
	stsc_tmp->entries = gf_malloc(sizeof(GF_StscEntry)*stsc_tmp->nb_entries);
	if (!stsc_tmp->entries) return GF_OUT_OF_MEM;
	//set write cache to last sample before unpack
	stsc_tmp->w_lastSampleNumber = stbl->SampleSize->sampleCount;
	stsc_tmp->w_lastChunkNumber = stbl->SampleSize->sampleCount;

	//OK write our two tables...
	ent = NULL;
	for (i = 0; i < stbl->SampleSize->sampleCount; i++) {
		//get the data info for the sample
		e = stbl_GetSampleInfos(stbl, i+1, &dataOffset, &chunkNumber, &sampleDescIndex, NULL);
		if (e) goto err_exit;
		ent = &stsc_tmp->entries[i];
		ent->isEdited = 0;
		ent->sampleDescriptionIndex = sampleDescIndex;
		//here's the trick: each sample is in ONE chunk
		ent->firstChunk = i+1;
		ent->nextChunk = i+2;
		ent->samplesPerChunk = 1;
		if (stco_tmp) {
			stco_tmp->offsets[i] = (u32) dataOffset;
		} else {
			co64_tmp->offsets[i] = dataOffset;
		}
	}
	//close the list
	if (ent) ent->nextChunk = 0;


	//done, remove our previous tables
	gf_list_del_item(stbl->child_boxes, stbl->ChunkOffset);
	gf_list_del_item(stbl->child_boxes, stbl->SampleToChunk);
	gf_isom_box_del(stbl->ChunkOffset);
	gf_isom_box_del((GF_Box *)stbl->SampleToChunk);
	//and set these ones...
	if (stco_tmp) {
		stbl->ChunkOffset = (GF_Box *)stco_tmp;
	} else {
		stbl->ChunkOffset = (GF_Box *)co64_tmp;
	}
	stbl->SampleToChunk = stsc_tmp;
	gf_list_add(stbl->child_boxes, stbl->ChunkOffset);
	gf_list_add(stbl->child_boxes, stbl->SampleToChunk);

	stbl->SampleToChunk->currentIndex = 0;
	stbl->SampleToChunk->currentChunk = 0;
	stbl->SampleToChunk->firstSampleInCurrentChunk = 0;
	return GF_OK;

err_exit:
	if (stco_tmp) gf_isom_box_del((GF_Box *) stco_tmp);
	if (co64_tmp) gf_isom_box_del((GF_Box *) co64_tmp);
	if (stsc_tmp) gf_isom_box_del((GF_Box *) stsc_tmp);
	return e;
}