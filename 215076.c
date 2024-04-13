GF_Err stbl_GetSampleRAP(GF_SyncSampleBox *stss, u32 SampleNumber, GF_ISOSAPType *IsRAP, u32 *prevRAP, u32 *nextRAP)
{
	u32 i;
	if (prevRAP) *prevRAP = 0;
	if (nextRAP) *nextRAP = 0;

	(*IsRAP) = RAP_NO;
	if (!stss || !SampleNumber) return GF_BAD_PARAM;

	if (stss->r_LastSyncSample && (stss->r_LastSyncSample < SampleNumber) ) {
		i = stss->r_LastSampleIndex;
	} else {
		i = 0;
	}
	for (; i < stss->nb_entries; i++) {
		//get the entry
		if (stss->sampleNumbers[i] == SampleNumber) {
			//update the cache
			stss->r_LastSyncSample = SampleNumber;
			stss->r_LastSampleIndex = i;
			(*IsRAP) = RAP;
		}
		else if (stss->sampleNumbers[i] > SampleNumber) {
			if (nextRAP) *nextRAP = stss->sampleNumbers[i];
			return GF_OK;
		}
		if (prevRAP) *prevRAP = stss->sampleNumbers[i];
	}
	return GF_OK;
}