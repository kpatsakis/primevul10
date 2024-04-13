GF_Err stbl_AddRAP(GF_SyncSampleBox *stss, u32 sampleNumber)
{
	u32 i, k;
	u32 *newNumbers;

	if (!stss || !sampleNumber) return GF_BAD_PARAM;

	if (stss->sampleNumbers == NULL) {
		ALLOC_INC(stss->alloc_size);
		stss->sampleNumbers = (u32*)gf_malloc(sizeof(u32)*stss->alloc_size);
		if (!stss->sampleNumbers) return GF_OUT_OF_MEM;
		stss->sampleNumbers[0] = sampleNumber;
		stss->nb_entries = 1;
		return GF_OK;
	}

	if (stss->sampleNumbers[stss->nb_entries-1] == sampleNumber) return GF_OK;

	if (stss->sampleNumbers[stss->nb_entries-1] < sampleNumber) {
		if (stss->nb_entries==stss->alloc_size) {
			ALLOC_INC(stss->alloc_size);
			stss->sampleNumbers = gf_realloc(stss->sampleNumbers, sizeof(u32) * stss->alloc_size);
			if (!stss->sampleNumbers) return GF_OUT_OF_MEM;
			memset(&stss->sampleNumbers[stss->nb_entries], 0, sizeof(u32) * (stss->alloc_size-stss->nb_entries) );
		}
		stss->sampleNumbers[stss->nb_entries] = sampleNumber;
	} else {
		newNumbers = (u32*)gf_malloc(sizeof(u32) * (stss->nb_entries + 1));
		if (!newNumbers) return GF_OUT_OF_MEM;
		//the table is in increasing order of sampleNumber
		k = 0;
		for (i = 0; i < stss->nb_entries; i++) {
			if (stss->sampleNumbers[i] >= sampleNumber) {
				newNumbers[i + k] = sampleNumber;
				k = 1;
			}
			newNumbers[i + k] = stss->sampleNumbers[i] + k;
		}
		gf_free(stss->sampleNumbers);
		stss->sampleNumbers = newNumbers;
		stss->alloc_size = stss->nb_entries+1;
	}
	//update our list
	stss->nb_entries ++;
	return GF_OK;
}