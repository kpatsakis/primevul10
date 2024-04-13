GF_Err stbl_RemoveRedundant(GF_SampleTableBox *stbl, u32 SampleNumber, u32 nb_samples)
{
	u32 i;

	if (!stbl->SampleDep) return GF_OK;
	if (stbl->SampleDep->sampleCount < SampleNumber) return GF_BAD_PARAM;
	if ((nb_samples>1) && (SampleNumber>1)) return GF_BAD_PARAM;

	if (nb_samples==1) {
		i = stbl->SampleDep->sampleCount - SampleNumber;
		if (i) memmove(&stbl->SampleDep->sample_info[SampleNumber-1], & stbl->SampleDep->sample_info[SampleNumber], sizeof(u8)*i);
		stbl->SampleDep->sample_info = (u8*)gf_realloc(stbl->SampleDep->sample_info, sizeof(u8) * (stbl->SampleDep->sampleCount-1));
		if (!stbl->SampleDep->sample_info) return GF_OUT_OF_MEM;
		stbl->SampleDep->sample_alloc = stbl->SampleDep->sampleCount-1;
		stbl->SampleDep->sampleCount-=1;
	} else {
		memmove(&stbl->SampleDep->sample_info[0], &stbl->SampleDep->sample_info[nb_samples], sizeof(u8) * (stbl->SampleDep->sampleCount - nb_samples) );
		stbl->SampleDep->sampleCount -= nb_samples;
	}
	return GF_OK;
}