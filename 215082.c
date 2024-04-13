GF_Err stbl_GetSampleDTS(GF_TimeToSampleBox *stts, u32 SampleNumber, u64 *DTS)
{
	return stbl_GetSampleDTS_and_Duration(stts, SampleNumber, DTS, NULL);
}