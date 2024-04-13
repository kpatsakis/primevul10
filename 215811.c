GF_Err stbl_SetPaddingBits(GF_SampleTableBox *stbl, u32 SampleNumber, u8 bits)
{
	u8 *p;
	//make sure the sample is a good one
	if (SampleNumber > stbl->SampleSize->sampleCount) return GF_BAD_PARAM;

	//create the table
	if (!stbl->PaddingBits) {
		stbl->PaddingBits = (GF_PaddingBitsBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_PADB);
		if (!stbl->PaddingBits) return GF_OUT_OF_MEM;
	}

	//alloc
	if (!stbl->PaddingBits->padbits || !stbl->PaddingBits->SampleCount) {
		stbl->PaddingBits->SampleCount = stbl->SampleSize->sampleCount;
		stbl->PaddingBits->padbits = (u8*)gf_malloc(sizeof(u8)*stbl->PaddingBits->SampleCount);
		if (!stbl->PaddingBits->padbits) return GF_OUT_OF_MEM;
		memset(stbl->PaddingBits->padbits, 0, sizeof(u8)*stbl->PaddingBits->SampleCount);
	}
	//realloc (this is needed in case n out of k samples get padding added)
	if (stbl->PaddingBits->SampleCount < stbl->SampleSize->sampleCount) {
		p = (u8*)gf_malloc(sizeof(u8) * stbl->SampleSize->sampleCount);
		if (!p) return GF_OUT_OF_MEM;
		//set everything to 0
		memset(p, 0, stbl->SampleSize->sampleCount);
		//copy our previous table
		memcpy(p, stbl->PaddingBits->padbits, stbl->PaddingBits->SampleCount);
		gf_free(stbl->PaddingBits->padbits);
		stbl->PaddingBits->padbits = p;
		stbl->PaddingBits->SampleCount = stbl->SampleSize->sampleCount;
	}
	stbl->PaddingBits->padbits[SampleNumber-1] = bits;
	return GF_OK;
}