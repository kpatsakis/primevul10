static GFINLINE GF_Err stbl_AddOffset(GF_SampleTableBox *stbl, GF_Box **old_stco, u64 offset)
{
	GF_ChunkOffsetBox *stco;
	GF_ChunkLargeOffsetBox *co64;
	u32 i;

	if ((*old_stco)->type == GF_ISOM_BOX_TYPE_STCO) {
		stco = (GF_ChunkOffsetBox *) *old_stco;
		//if dataOffset is bigger than 0xFFFFFFFF, move to LARGE offset
		if (offset > 0xFFFFFFFF) {
			s32 prev_pos = gf_list_find(stbl->child_boxes, *old_stco);
			co64 = (GF_ChunkLargeOffsetBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_CO64);
			if (!co64) return GF_OUT_OF_MEM;
			co64->nb_entries = stco->nb_entries + 1;
			co64->alloc_size = co64->nb_entries;
			co64->offsets = (u64*)gf_malloc(co64->nb_entries * sizeof(u64));
			if (!co64->offsets) {
				gf_isom_box_del((GF_Box *)co64);
				return GF_OUT_OF_MEM;
			}
			for (i = 0; i< co64->nb_entries - 1; i++) {
				co64->offsets[i] = (u64) stco->offsets[i];
			}
			co64->offsets[i] = offset;
			//delete the box...
			gf_isom_box_del_parent(&stbl->child_boxes, *old_stco);
			*old_stco = (GF_Box *)co64;

			assert (stbl->child_boxes);
			//register new box only if old one was registered
			if (prev_pos>=0)
				gf_list_insert(stbl->child_boxes, *old_stco, prev_pos);
			return GF_OK;
		}
		//OK, stick with regular...
		if (stco->nb_entries==stco->alloc_size) {
			ALLOC_INC(stco->alloc_size);
			stco->offsets = (u32*)gf_realloc(stco->offsets, stco->alloc_size * sizeof(u32));
			if (!stco->offsets) return GF_OUT_OF_MEM;
			memset(&stco->offsets[stco->nb_entries], 0, (stco->alloc_size - stco->nb_entries) * sizeof(u32));
		}

		stco->offsets[stco->nb_entries] = (u32) offset;
		stco->nb_entries += 1;
	} else {
		//this is a large offset
		co64 = (GF_ChunkLargeOffsetBox *) *old_stco;
		if (co64->nb_entries==co64->alloc_size) {
			ALLOC_INC(co64->alloc_size);
			co64->offsets = (u64*)gf_realloc(co64->offsets, co64->alloc_size * sizeof(u64));
			if (!co64->offsets) return GF_OUT_OF_MEM;
			memset(&co64->offsets[co64->nb_entries], 0, (co64->alloc_size - co64->nb_entries) * sizeof(u64) );
		}
		co64->offsets[co64->nb_entries] = offset;
		co64->nb_entries += 1;
	}
	return GF_OK;
}