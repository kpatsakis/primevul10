GF_Err stbl_AppendRAP(GF_SampleTableBox *stbl, u8 isRap)
{
	u32 i;

	//no sync table
	if (!stbl->SyncSample) {
		//all samples RAP - no table
		if (isRap) return GF_OK;

		//nope, create one
		stbl->SyncSample = (GF_SyncSampleBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSS);
		if (!stbl->SyncSample) return GF_OUT_OF_MEM;

		if (stbl->SampleSize->sampleCount > 1) {
			stbl->SyncSample->sampleNumbers = (u32*)gf_malloc(sizeof(u32) * (stbl->SampleSize->sampleCount-1));
			if (!stbl->SyncSample->sampleNumbers) return GF_OUT_OF_MEM;
			for (i=0; i<stbl->SampleSize->sampleCount-1; i++)
				stbl->SyncSample->sampleNumbers[i] = i+1;

		}
		stbl->SyncSample->nb_entries = stbl->SampleSize->sampleCount-1;
		stbl->SyncSample->alloc_size = stbl->SyncSample->nb_entries;
		return GF_OK;
	}
	if (!isRap) return GF_OK;

	if (stbl->SyncSample->alloc_size == stbl->SyncSample->nb_entries) {
		ALLOC_INC(stbl->SyncSample->alloc_size);
		stbl->SyncSample->sampleNumbers = (u32*) gf_realloc(stbl->SyncSample->sampleNumbers, sizeof(u32) * stbl->SyncSample->alloc_size);
		if (!stbl->SyncSample->sampleNumbers) return GF_OUT_OF_MEM;
		memset(&stbl->SyncSample->sampleNumbers[stbl->SyncSample->nb_entries], 0, sizeof(u32) * (stbl->SyncSample->alloc_size-stbl->SyncSample->nb_entries) );
	}
	stbl->SyncSample->sampleNumbers[stbl->SyncSample->nb_entries] = stbl->SampleSize->sampleCount;
	stbl->SyncSample->nb_entries += 1;
	return GF_OK;
}