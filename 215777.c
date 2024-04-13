GF_Err stbl_SetSampleRAP(GF_SyncSampleBox *stss, u32 SampleNumber, u8 isRAP)
{
	u32 i;

	//check if we have already a sync sample
	for (i = 0; i < stss->nb_entries; i++) {

		if (stss->sampleNumbers[i] < SampleNumber) continue;
		else if (stss->sampleNumbers[i] > SampleNumber) break;

		/*found our sample number*/
		if (isRAP) return GF_OK;
		/*remove it...*/
		if (i+1 < stss->nb_entries)
			memmove(stss->sampleNumbers + i, stss->sampleNumbers + i + 1, sizeof(u32) * (stss->nb_entries - i - 1));
		stss->nb_entries--;
		return GF_OK;
	}
	//we need to insert a RAP somewhere if RAP ...
	if (!isRAP) return GF_OK;
	if (stss->nb_entries==stss->alloc_size) {
		ALLOC_INC(stss->alloc_size);
		stss->sampleNumbers = gf_realloc(stss->sampleNumbers, sizeof(u32)*stss->alloc_size);
		if (!stss->sampleNumbers) return GF_OUT_OF_MEM;
		memset(&stss->sampleNumbers[stss->nb_entries], 0, sizeof(u32)*(stss->alloc_size - stss->nb_entries) );
	}

	if (i+1 < stss->nb_entries)
		memmove(stss->sampleNumbers + i + 1, stss->sampleNumbers + i, sizeof(u32) * (stss->nb_entries - i - 1));
	stss->sampleNumbers[i] = SampleNumber;
	stss->nb_entries ++;
	return GF_OK;
}