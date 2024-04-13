GF_Err stbl_unpackCTS(GF_SampleTableBox *stbl)
{
	GF_DttsEntry *packed;
	u32 i, j, count;
	GF_CompositionOffsetBox *ctts;
	ctts = stbl->CompositionOffset;
	if (!ctts || ctts->unpack_mode) return GF_OK;
	ctts->unpack_mode = 1;

	packed = ctts->entries;
	count = ctts->nb_entries;
	ctts->entries = NULL;
	ctts->nb_entries = 0;
	ctts->alloc_size = 0;
	for (i=0; i<count; i++) {
		for (j=0; j<packed[i].sampleCount; j++) {
			if (ctts->nb_entries == ctts->alloc_size) {
				ALLOC_INC(ctts->alloc_size);
				ctts->entries = gf_realloc(ctts->entries, sizeof(GF_DttsEntry)*ctts->alloc_size);
				if (!ctts->entries) return GF_OUT_OF_MEM;

				memset(&ctts->entries[ctts->nb_entries], 0, sizeof(GF_DttsEntry)*(ctts->alloc_size-ctts->nb_entries) );
			}
			ctts->entries[ctts->nb_entries].decodingOffset = packed[i].decodingOffset;
			ctts->entries[ctts->nb_entries].sampleCount = 1;
			ctts->nb_entries++;
		}
	}
	gf_free(packed);

	while (stbl->SampleSize->sampleCount > ctts->nb_entries) {
		if (ctts->nb_entries == ctts->alloc_size) {
			ALLOC_INC(ctts->alloc_size);
			ctts->entries = gf_realloc(ctts->entries, sizeof(GF_DttsEntry)*ctts->alloc_size);
			if (!ctts->entries) return GF_OUT_OF_MEM;
			memset(&ctts->entries[ctts->nb_entries], 0, sizeof(GF_DttsEntry)*(ctts->alloc_size-ctts->nb_entries) );
		}
		ctts->entries[ctts->nb_entries].decodingOffset = 0;
		ctts->entries[ctts->nb_entries].sampleCount = 1;
		ctts->nb_entries++;
	}
	return GF_OK;
}