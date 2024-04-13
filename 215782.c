GF_Err stbl_SetSampleSize(GF_SampleSizeBox *stsz, u32 SampleNumber, u32 size)
{
	u32 i;
	if (!SampleNumber || (stsz->sampleCount < SampleNumber)) return GF_BAD_PARAM;

	if (stsz->sampleSize) {
		if (stsz->sampleSize == size) return GF_OK;
		if (stsz->sampleCount == 1) {
			stsz->sampleSize = size;
			return GF_OK;
		}
		//nope, we have to rewrite a table
		stsz->sizes = (u32*)gf_malloc(sizeof(u32)*stsz->sampleCount);
		if (!stsz->sizes) return GF_OUT_OF_MEM;
		for (i=0; i<stsz->sampleCount; i++) stsz->sizes[i] = stsz->sampleSize;
		stsz->sampleSize = 0;
	}
	stsz->sizes[SampleNumber - 1] = size;
	return GF_OK;
}