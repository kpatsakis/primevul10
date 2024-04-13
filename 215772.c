GF_Err stbl_SetDependencyType(GF_SampleTableBox *stbl, u32 sampleNumber, u32 isLeading, u32 dependsOn, u32 dependedOn, u32 redundant)
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

	if (sdtp->sampleCount < sampleNumber) {
		u32 i;
		sdtp->sample_info = (u8*) gf_realloc(sdtp->sample_info, sizeof(u8) * sampleNumber);
		if (!sdtp->sample_info) return GF_OUT_OF_MEM;
		sdtp->sample_alloc = sampleNumber;

		for (i=sdtp->sampleCount; i<sampleNumber; i++) {
			sdtp->sample_info[i] = 0;
		}
		sdtp->sampleCount = sampleNumber;
	}
	sdtp->sample_info[sampleNumber-1] = flags;
	return GF_OK;
}