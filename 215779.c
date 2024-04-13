GF_Err stbl_AddRedundant(GF_SampleTableBox *stbl, u32 sampleNumber)
{
	GF_SampleDependencyTypeBox *sdtp;

	if (stbl->SampleDep == NULL) {
		stbl->SampleDep = (GF_SampleDependencyTypeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_SDTP);
		if (!stbl->SampleDep) return GF_OUT_OF_MEM;
	}
	sdtp = stbl->SampleDep;
	if (sdtp->sampleCount + 1 < sampleNumber) {
		u32 missed = sampleNumber-1 - sdtp->sampleCount;
		sdtp->sample_info = (u8*) gf_realloc(sdtp->sample_info, sizeof(u8) * (sdtp->sampleCount+missed) );
		if (!sdtp->sample_info) return GF_OUT_OF_MEM;
		sdtp->sample_alloc = sdtp->sampleCount+missed;
		memset(&sdtp->sample_info[sdtp->sampleCount], 0, sizeof(u8) * missed );
		while (missed) {
			GF_ISOSAPType isRAP;
			if (stbl->SyncSample) stbl_GetSampleRAP(stbl->SyncSample, sdtp->sampleCount+1, &isRAP, NULL, NULL);
			else isRAP = 1;
			sdtp->sample_info[sdtp->sampleCount] = isRAP ? 0x20 : 0;
			sdtp->sampleCount++;
			missed--;
		}
	}

	sdtp->sample_info = (u8*) gf_realloc(sdtp->sample_info, sizeof(u8) * (sdtp->sampleCount + 1));
	if (!sdtp->sample_info) return GF_OUT_OF_MEM;
	sdtp->sample_alloc = sdtp->sampleCount+1;
	if (sdtp->sampleCount < sampleNumber) {
		sdtp->sample_info[sdtp->sampleCount] = 0x29;
	} else {
		u32 snum = sampleNumber-1;
		memmove(sdtp->sample_info+snum+1, sdtp->sample_info+snum, sizeof(u8) * (sdtp->sampleCount - snum) );
		sdtp->sample_info[snum] = 0x29;
	}
	//update our list
	sdtp->sampleCount ++;
	return GF_OK;
}