GF_Err gf_isom_refresh_size_info(GF_ISOFile *file, u32 trackNumber)
{
	u32 i, size;
	GF_TrackBox *trak;
	GF_SampleSizeBox *stsz;
	trak = gf_isom_get_track_from_file(file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsz = trak->Media->information->sampleTable->SampleSize;
	if (stsz->sampleSize || !stsz->sampleCount) return GF_OK;

	size = stsz->sizes[0];
	for (i=1; i<stsz->sampleCount; i++) {
		if (stsz->sizes[i] != size) {
			size = 0;
			break;
		}
	}
	if (size) {
		gf_free(stsz->sizes);
		stsz->sizes = NULL;
		stsz->sampleSize = size;
	}
	return GF_OK;
}