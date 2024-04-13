GF_Err stbl_GetSampleDepType(GF_SampleDependencyTypeBox *sdep, u32 SampleNumber, u32 *isLeading, u32 *dependsOn, u32 *dependedOn, u32 *redundant)
{
	u8 flag;

	assert(dependsOn && dependedOn && redundant);
	*dependsOn = *dependedOn = *redundant = 0;

	if (SampleNumber > sdep->sampleCount) {
		return GF_OK;
	}
	flag = sdep->sample_info[SampleNumber-1];
	*isLeading = (flag >> 6) & 3;
	*dependsOn = (flag >> 4) & 3;
	*dependedOn = (flag >> 2) & 3;
	*redundant = (flag) & 3;
	return GF_OK;
}