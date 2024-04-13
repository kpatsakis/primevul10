GF_Err stbl_AddCTS(GF_SampleTableBox *stbl, u32 sampleNumber, s32 offset)
{
	u32 i, j, sampNum, *CTSs;

	GF_CompositionOffsetBox *ctts = stbl->CompositionOffset;

	/*in unpack mode we're sure to have 1 ctts entry per sample*/
	if (ctts->unpack_mode) {
		if (ctts->nb_entries==ctts->alloc_size) {
			ALLOC_INC(ctts->alloc_size);
			ctts->entries = gf_realloc(ctts->entries, sizeof(GF_DttsEntry)*ctts->alloc_size);
			if (!ctts->entries) return GF_OUT_OF_MEM;
			memset(&ctts->entries[ctts->nb_entries], 0, sizeof(GF_DttsEntry)*(ctts->alloc_size - ctts->nb_entries) );
		}
		ctts->entries[ctts->nb_entries].decodingOffset = offset;
		ctts->entries[ctts->nb_entries].sampleCount = 1;
		ctts->nb_entries++;
		ctts->w_LastSampleNumber++;
		if (offset<0) ctts->version=1;
		return GF_OK;
	}
	//check if we're working in order...
	if (ctts->w_LastSampleNumber < sampleNumber) {
		//add some 0 till we get to the sample
		while (ctts->w_LastSampleNumber + 1 != sampleNumber) {
			GF_Err e = AddCompositionOffset(ctts, 0);
			if (e) return e;
		}
		return AddCompositionOffset(ctts, offset);
	}

	//NOPE we are inserting a sample...
	CTSs = (u32*)gf_malloc(sizeof(u32) * (stbl->SampleSize->sampleCount+1) );
	if (!CTSs) return GF_OUT_OF_MEM;
	sampNum = 0;
	for (i=0; i<ctts->nb_entries; i++) {
		for (j = 0; j<ctts->entries[i].sampleCount; j++) {
			if (sampNum > stbl->SampleSize->sampleCount) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Too many CTS Offset entries for %d samples\n", stbl->SampleSize->sampleCount ));
				gf_free(CTSs);
				return GF_ISOM_INVALID_FILE;
			}
			if (sampNum+1==sampleNumber) {
				CTSs[sampNum] = offset;
				sampNum ++;
			}
			CTSs[sampNum] = ctts->entries[i].decodingOffset;
			sampNum ++;
		}
	}

	/*we will at most add 2 new entries (splitting of an existing one)*/
	if (ctts->nb_entries+2>=ctts->alloc_size) {
		ctts->alloc_size += 2;
		ctts->entries = gf_realloc(ctts->entries, sizeof(GF_DttsEntry)*ctts->alloc_size);
		if (!ctts->entries) return GF_OUT_OF_MEM;
		memset(&ctts->entries[ctts->nb_entries], 0, sizeof(GF_DttsEntry)*(ctts->alloc_size-ctts->nb_entries) );
	}

	ctts->entries[0].sampleCount = 1;
	ctts->entries[0].decodingOffset = CTSs[0];
	ctts->nb_entries = 1;
	j=0;
	for (i=1; i<stbl->SampleSize->sampleCount + 1; i++) {
		if (CTSs[i]==ctts->entries[j].decodingOffset) {
			ctts->entries[j].sampleCount++;
		} else {
			j++;
			ctts->nb_entries++;
			ctts->entries[j].sampleCount = 1;
			ctts->entries[j].decodingOffset = CTSs[i];
		}
	}
	gf_free(CTSs);

	if (offset<0) ctts->version=1;

	/*we've inserted a sample, therefore the last sample (n) has now number n+1
	we cannot use SampleCount because we have probably skipped some samples
	(we're calling AddCTS only if the sample has a offset !!!)*/
	ctts->w_LastSampleNumber += 1;
	return GF_OK;
}