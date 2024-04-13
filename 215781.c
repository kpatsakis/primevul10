GF_Err stbl_repackCTS(GF_CompositionOffsetBox *ctts)
{
	u32 i, j;

	if (!ctts->unpack_mode) return GF_OK;
	ctts->unpack_mode = 0;

	j=0;
	for (i=1; i<ctts->nb_entries; i++) {
		if (ctts->entries[i].decodingOffset==ctts->entries[j].decodingOffset) {
			ctts->entries[j].sampleCount++;
		} else {
			j++;
			ctts->entries[j].sampleCount = 1;
			ctts->entries[j].decodingOffset = ctts->entries[i].decodingOffset;
		}
	}
	ctts->nb_entries=j+1;
	/*note we don't realloc*/
	return GF_OK;
}