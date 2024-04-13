GF_Err stbl_AppendDependencyType(GF_SampleTableBox *stbl, u32 isLeading, u32 dependsOn, u32 dependedOn, u32 redundant)
{
	GF_SampleDependencyTypeBox *sdtp;
	u32 flags;
	if (stbl->SampleDep == NULL) {
		stbl->SampleDep = (GF_SampleDependencyTypeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_SDTP);
		if (!stbl->SampleDep) return GF_OUT_OF_MEM;
	}
	sdtp = stbl->SampleDep;

	flags = 0;
	flags |= isLeading << 6;
	flags |= dependsOn << 4;
	flags |= dependedOn << 2;
	flags |= redundant;

	if (sdtp->sampleCount >= sdtp->sample_alloc) {
		ALLOC_INC(sdtp->sample_alloc);
		if (sdtp->sampleCount >= sdtp->sample_alloc) sdtp->sample_alloc = sdtp->sampleCount+1;
		sdtp->sample_info = (u8*) gf_realloc(sdtp->sample_info, sizeof(u8) * sdtp->sample_alloc);
		if (!sdtp->sample_info) return GF_OUT_OF_MEM;
	}
	sdtp->sample_info[sdtp->sampleCount] = flags;
	sdtp->sampleCount ++;
	return GF_OK;
}