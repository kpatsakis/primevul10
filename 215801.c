GF_Err stbl_SampleSizeAppend(GF_SampleSizeBox *stsz, u32 data_size)
{
	u32 i;
	if (!stsz || !stsz->sampleCount) return GF_BAD_PARAM;

	//we must realloc our table
	if (stsz->sampleSize) {
		stsz->sizes = (u32*)gf_malloc(sizeof(u32)*stsz->sampleCount);
		if (!stsz->sizes) return GF_OUT_OF_MEM;
		for (i=0; i<stsz->sampleCount; i++) stsz->sizes[i] = stsz->sampleSize;
		stsz->sampleSize = 0;
	}
	if (!stsz->sizes) {
		stsz->sampleSize = data_size;
	} else {
		u32 single_size;
		stsz->sizes[stsz->sampleCount-1] += data_size;

		single_size = stsz->sizes[0];
		for (i=1; i<stsz->sampleCount; i++) {
			if (stsz->sizes[i] != single_size) {
				single_size = 0;
				break;
			}
		}
		if (single_size) {
			stsz->sampleSize = single_size;
			gf_free(stsz->sizes);
			stsz->sizes = NULL;
		}
	}
	return GF_OK;
}