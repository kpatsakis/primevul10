GF_Err stbl_AppendCTSOffset(GF_SampleTableBox *stbl, s32 offset)
{
	GF_CompositionOffsetBox *ctts;

	if (!stbl->CompositionOffset) {
		stbl->CompositionOffset = (GF_CompositionOffsetBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_CTTS);
		if (!stbl->CompositionOffset) return GF_OUT_OF_MEM;
	}
	ctts = stbl->CompositionOffset;
	ctts->w_LastSampleNumber ++;

	if (!ctts->unpack_mode && ctts->nb_entries && (ctts->entries[ctts->nb_entries-1].decodingOffset == offset) ) {
		ctts->entries[ctts->nb_entries-1].sampleCount++;
		return GF_OK;
	}
	if (ctts->nb_entries==ctts->alloc_size) {
		ALLOC_INC(ctts->alloc_size);
		ctts->entries = gf_realloc(ctts->entries, sizeof(GF_DttsEntry)*ctts->alloc_size);
		if (!ctts->entries) return GF_OUT_OF_MEM;
		memset(&ctts->entries[ctts->nb_entries], 0, sizeof(GF_DttsEntry)*(ctts->alloc_size-ctts->nb_entries) );
	}
	ctts->entries[ctts->nb_entries].decodingOffset = offset;
	ctts->entries[ctts->nb_entries].sampleCount = 1;
	ctts->nb_entries++;
	if (offset<0) ctts->version=1;

	if (ABS(offset) > ctts->max_ts_delta) ctts->max_ts_delta = ABS(offset);

	return GF_OK;
}