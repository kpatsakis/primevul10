GF_Err stbl_AppendPadding(GF_SampleTableBox *stbl, u8 padding)
{
	if (!stbl->PaddingBits) {
		stbl->PaddingBits = (GF_PaddingBitsBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_PADB);
		if (!stbl->PaddingBits) return GF_OUT_OF_MEM;
	}
	stbl->PaddingBits->padbits = (u8*)gf_realloc(stbl->PaddingBits->padbits, sizeof(u8) * stbl->SampleSize->sampleCount);
	if (!stbl->PaddingBits->padbits) return GF_OUT_OF_MEM;
	stbl->PaddingBits->padbits[stbl->SampleSize->sampleCount-1] = padding;
	stbl->PaddingBits->SampleCount = stbl->SampleSize->sampleCount;
	return GF_OK;
}