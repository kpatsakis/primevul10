GF_Err stbl_SetChunkOffset(GF_MediaBox *mdia, u32 sampleNumber, u64 offset)
{
	GF_StscEntry *ent;
	u32 i;
	GF_ChunkLargeOffsetBox *co64;
	GF_SampleTableBox *stbl = mdia->information->sampleTable;

	if (!sampleNumber || !stbl) return GF_BAD_PARAM;

	ent = &stbl->SampleToChunk->entries[sampleNumber - 1];

	//we edit our entry if self contained
	if (Media_IsSelfContained(mdia, ent->sampleDescriptionIndex))
		ent->isEdited = 1;

	//and we change our offset
	if (stbl->ChunkOffset->type == GF_ISOM_BOX_TYPE_STCO) {
		//if the new offset is a large one, we have to rewrite our table...
		if (offset > 0xFFFFFFFF) {
			co64 = (GF_ChunkLargeOffsetBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_CO64);
			if (!co64) return GF_OUT_OF_MEM;
			co64->nb_entries = ((GF_ChunkOffsetBox *)stbl->ChunkOffset)->nb_entries;
			co64->alloc_size = co64->nb_entries;
			co64->offsets = (u64*)gf_malloc(sizeof(u64)*co64->nb_entries);
			if (!co64->offsets) return GF_OUT_OF_MEM;
			for (i=0; i<co64->nb_entries; i++) {
				co64->offsets[i] = (u64) ((GF_ChunkOffsetBox *)stbl->ChunkOffset)->offsets[i];
			}
			co64->offsets[ent->firstChunk - 1] = offset;
			gf_isom_box_del_parent(&stbl->child_boxes, stbl->ChunkOffset);
			stbl->ChunkOffset = (GF_Box *) co64;
			return GF_OK;
		}
		((GF_ChunkOffsetBox *)stbl->ChunkOffset)->offsets[ent->firstChunk - 1] = (u32) offset;
	} else {
		((GF_ChunkLargeOffsetBox *)stbl->ChunkOffset)->offsets[ent->firstChunk - 1] = offset;
	}
	return GF_OK;
}