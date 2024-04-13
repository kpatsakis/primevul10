GF_Err stbl_RemovePaddingBits(GF_SampleTableBox *stbl, u32 SampleNumber)
{
	u8 *p;
	u32 i, k;

	if (!stbl->PaddingBits) return GF_OK;
	if (stbl->PaddingBits->SampleCount < SampleNumber) return GF_BAD_PARAM;

	//last sample - remove the table
	if (stbl->PaddingBits->SampleCount == 1) {
		gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) stbl->PaddingBits);
		stbl->PaddingBits = NULL;
		return GF_OK;
	}

	//reallocate and check size by the way...
	p = (u8 *)gf_malloc(sizeof(u8) * (stbl->PaddingBits->SampleCount - 1));
	if (!p) return GF_OUT_OF_MEM;

	k=0;
	for (i=0; i<stbl->PaddingBits->SampleCount; i++) {
		if (i+1 != SampleNumber) {
			p[k] = stbl->PaddingBits->padbits[i];
			k++;
		}
	}

	stbl->PaddingBits->SampleCount -= 1;
	gf_free(stbl->PaddingBits->padbits);
	stbl->PaddingBits->padbits = p;
	return GF_OK;
}