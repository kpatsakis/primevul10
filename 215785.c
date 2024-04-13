GF_Err stbl_SetRedundant(GF_SampleTableBox *stbl, u32 sampleNumber)
{
	if (stbl->SampleDep->sampleCount < sampleNumber) {
		return stbl_AddRedundant(stbl, sampleNumber);
	} else {
		stbl->SampleDep->sample_info[sampleNumber-1] = 0x29;
		return GF_OK;
	}
}