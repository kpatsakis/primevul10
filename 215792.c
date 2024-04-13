GF_Err AddCompositionOffset(GF_CompositionOffsetBox *ctts, s32 offset)
{
	if (!ctts) return GF_BAD_PARAM;

	if (ctts->nb_entries && (ctts->entries[ctts->nb_entries-1].decodingOffset==offset)) {
		ctts->entries[ctts->nb_entries-1].sampleCount++;
	} else {
		if (ctts->alloc_size==ctts->nb_entries) {
			ALLOC_INC(ctts->alloc_size);
			ctts->entries = gf_realloc(ctts->entries, sizeof(GF_DttsEntry)*ctts->alloc_size);
			if (!ctts->entries) return GF_OUT_OF_MEM;
			memset(&ctts->entries[ctts->nb_entries], 0, sizeof(GF_DttsEntry)*(ctts->alloc_size-ctts->nb_entries) );
		}
		if (!ctts->entries) return GF_OUT_OF_MEM;

		ctts->entries[ctts->nb_entries].decodingOffset = offset;
		ctts->entries[ctts->nb_entries].sampleCount = 1;
		ctts->nb_entries++;
	}
	if (offset<0) ctts->version=1;
	ctts->w_LastSampleNumber++;
	return GF_OK;
}