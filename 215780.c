GF_Err stbl_AppendDepType(GF_SampleTableBox *stbl, u32 DepType)
{
	if (!stbl->SampleDep) {
		stbl->SampleDep = (GF_SampleDependencyTypeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_SDTP);
		if (!stbl->SampleDep) return GF_OUT_OF_MEM;
	}
	stbl->SampleDep->sample_info = (u8*)gf_realloc(stbl->SampleDep->sample_info, sizeof(u8)*stbl->SampleSize->sampleCount );
	if (!stbl->SampleDep->sample_info) return GF_OUT_OF_MEM;
	stbl->SampleDep->sample_alloc = stbl->SampleSize->sampleCount;
	stbl->SampleDep->sample_info[stbl->SampleDep->sampleCount] = DepType;
	stbl->SampleDep->sampleCount = stbl->SampleSize->sampleCount;
	return GF_OK;
}