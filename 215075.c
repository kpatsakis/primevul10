GF_Err stbl_GetSampleSize(GF_SampleSizeBox *stsz, u32 SampleNumber, u32 *Size)
{
	if (!stsz || !SampleNumber || (SampleNumber > stsz->sampleCount))
		return GF_BAD_PARAM;

	if (stsz->sampleSize && (stsz->type != GF_ISOM_BOX_TYPE_STZ2)) {
		(*Size) = stsz->sampleSize;
	} else if (stsz->sizes) {
		(*Size) = stsz->sizes[SampleNumber - 1];
	} else {
		(*Size) = 0;
	}
	return GF_OK;
}