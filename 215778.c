GF_Err stbl_RemoveRAP(GF_SampleTableBox *stbl, u32 sampleNumber)
{
	u32 i;

	GF_SyncSampleBox *stss = stbl->SyncSample;
	if (!stss) return GF_OK;

	//we remove the only one around...
	if (stss->nb_entries == 1) {
		if (stss->sampleNumbers[0] != sampleNumber) return GF_OK;
		//free our numbers but don't delete (all samples are NON-sync
		gf_free(stss->sampleNumbers);
		stss->sampleNumbers = NULL;
		stss->r_LastSampleIndex = stss->r_LastSyncSample = 0;
		stss->alloc_size = stss->nb_entries = 0;
		return GF_OK;
	}

	for (i=0; i<stss->nb_entries; i++) {
		//found the sample
		if (sampleNumber == stss->sampleNumbers[i]) {
			memmove(&stss->sampleNumbers[i], &stss->sampleNumbers[i+1], sizeof(u32)* (stss->nb_entries-i-1) );
			stss->nb_entries--;
		}

		if (sampleNumber < stss->sampleNumbers[i]) {
			assert(stss->sampleNumbers[i]);
			stss->sampleNumbers[i]--;
		}
	}
	return GF_OK;
}