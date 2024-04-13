GF_Err stbl_RemoveSize(GF_SampleTableBox *stbl, u32 sampleNumber, u32 nb_samples)
{
	GF_SampleSizeBox *stsz = stbl->SampleSize;

	if ((nb_samples>1) && (sampleNumber>1)) return GF_BAD_PARAM;
	//last sample
	if (stsz->sampleCount == 1) {
		if (stsz->sizes) gf_free(stsz->sizes);
		stsz->sizes = NULL;
		stsz->sampleCount = 0;
		return GF_OK;
	}
	//one single size
	if (stsz->sampleSize) {
		stsz->sampleCount -= nb_samples;
		return GF_OK;
	}
	if (nb_samples==1) {
		if (sampleNumber < stsz->sampleCount)
			memmove(stsz->sizes + sampleNumber - 1, stsz->sizes + sampleNumber, sizeof(u32) * (stsz->sampleCount - sampleNumber));
	} else {
		if (nb_samples < stsz->sampleCount)
			memmove(stsz->sizes, stsz->sizes + nb_samples, sizeof(u32) * (stsz->sampleCount - nb_samples));
	}
	stsz->sampleCount -= nb_samples;
	return GF_OK;
}