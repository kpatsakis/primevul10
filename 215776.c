GF_Err stbl_AppendChunk(GF_SampleTableBox *stbl, u64 offset)
{
	GF_ChunkOffsetBox *stco;
	GF_ChunkLargeOffsetBox *co64;
	u32 i;
	
	//we may have to convert the table...
	if (stbl->ChunkOffset->type==GF_ISOM_BOX_TYPE_STCO) {
		stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;

		if (offset>0xFFFFFFFF) {
			co64 = (GF_ChunkLargeOffsetBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_CO64);
			if (!co64) return GF_OUT_OF_MEM;
			co64->nb_entries = stco->nb_entries + 1;
			if (co64->nb_entries<=stco->nb_entries) return GF_OUT_OF_MEM;
			co64->alloc_size = co64->nb_entries;
			co64->offsets = (u64*)gf_malloc(sizeof(u64) * co64->nb_entries);
			if (!co64->offsets) return GF_OUT_OF_MEM;
			for (i=0; i<stco->nb_entries; i++) co64->offsets[i] = stco->offsets[i];
			co64->offsets[i] = offset;
			gf_isom_box_del_parent(&stbl->child_boxes, stbl->ChunkOffset);
			stbl->ChunkOffset = (GF_Box *) co64;
			return GF_OK;
		}
		//we're fine
		stco->alloc_size = stco->nb_entries + 1;
		if (stco->alloc_size < stco->nb_entries + 1) return GF_OUT_OF_MEM;
		stco->offsets = gf_realloc(stco->offsets, sizeof(u32)*stco->alloc_size);
		if (!stco->offsets) return GF_OUT_OF_MEM;
		stco->offsets[stco->nb_entries] = (u32) offset;
		stco->nb_entries += 1;
		return GF_OK;
	}

	co64 = (GF_ChunkLargeOffsetBox *)stbl->ChunkOffset;
	co64->alloc_size = co64->nb_entries+1;
	if (co64->alloc_size < co64->nb_entries + 1) return GF_OUT_OF_MEM;

	co64->offsets = gf_realloc(co64->offsets, sizeof(u64)*co64->alloc_size);
	if (!co64->offsets) return GF_OUT_OF_MEM;
	co64->offsets[co64->nb_entries] = offset;
	co64->alloc_size = co64->nb_entries;
	return GF_OK;
}