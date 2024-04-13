GF_Err stbl_findEntryForTime(GF_SampleTableBox *stbl, u64 DTS, u8 useCTS, u32 *sampleNumber, u32 *prevSampleNumber)
{
	u32 i, j, curSampNum, count;
	s32 CTSOffset;
	u64 curDTS;
	GF_SttsEntry *ent;
	(*sampleNumber) = 0;
	(*prevSampleNumber) = 0;

	if (!stbl->TimeToSample) return GF_ISOM_INVALID_FILE;

	/*CTS is ALWAYS disabled for now to make sure samples are fetched in decoding order. useCTS is therefore disabled*/
#if 0
	if (!stbl->CompositionOffset) useCTS = 0;
#endif

	//our cache
	if (stbl->TimeToSample->r_FirstSampleInEntry &&
	        (DTS >= stbl->TimeToSample->r_CurrentDTS) ) {
		//if we're using CTS, we don't really know whether we're in the good entry or not
		//(eg, the real DTS of the sample could be in a previous entry
		i = stbl->TimeToSample->r_currentEntryIndex;
		curDTS = stbl->TimeToSample->r_CurrentDTS;
		curSampNum = stbl->TimeToSample->r_FirstSampleInEntry;
	} else {
		i = 0;
		curDTS = stbl->TimeToSample->r_CurrentDTS = 0;
		curSampNum = stbl->TimeToSample->r_FirstSampleInEntry = 1;
		stbl->TimeToSample->r_currentEntryIndex = 0;
	}

#if 0
	//we need to validate our cache if we are using CTS because of B-frames and co...
	if (i && useCTS) {
		while (1) {
			stbl_GetSampleCTS(stbl->CompositionOffset, curSampNum, &CTSOffset);
			//we're too far, rewind
			if ( i && (curDTS + CTSOffset > DTS) ) {
				ent = &stbl->TimeToSample->entries[i];
				curSampNum -= ent->sampleCount;
				curDTS -= (u64)ent->sampleDelta * ent->sampleCount;
				i --;
			} else if (!i) {
				//beginning of the table, no choice
				curDTS = stbl->TimeToSample->r_CurrentDTS = 0;
				curSampNum = stbl->TimeToSample->r_FirstSampleInEntry = 1;
				stbl->TimeToSample->r_currentEntryIndex = 0;
				break;
			} else {
				//OK now we're good
				break;
			}
		}
	}
#endif

	//look for the DTS from this entry
	count = stbl->TimeToSample->nb_entries;
	for (; i<count; i++) {
		ent = &stbl->TimeToSample->entries[i];
#if 0
		if (useCTS) {
			stbl_GetSampleCTS(stbl->CompositionOffset, curSampNum, &CTSOffset);
		} else
#endif
		{
			CTSOffset = 0;
		}
		for (j=0; j<ent->sampleCount; j++) {
			if (curDTS + CTSOffset >= DTS) goto entry_found;
			curSampNum += 1;
			curDTS += ent->sampleDelta;
		}
		//we're switching to the next entry, update the cache!
		stbl->TimeToSample->r_CurrentDTS += (u64)ent->sampleCount * ent->sampleDelta;
		stbl->TimeToSample->r_currentEntryIndex += 1;
		stbl->TimeToSample->r_FirstSampleInEntry += ent->sampleCount;
	}
	//return as is
	return GF_OK;

entry_found:
	//do we have the exact time ?
	if (curDTS + CTSOffset == DTS) {
		(*sampleNumber) = curSampNum;
	}
	//if we match the exact DTS also select this sample
	else if (curDTS == DTS) {
		(*sampleNumber) = curSampNum;
	} else {
		//exception for the first sample (we need to "load" the playback)
		if (curSampNum != 1) {
			(*prevSampleNumber) = curSampNum - 1;
		} else {
			(*prevSampleNumber) = 1;
		}
	}
	return GF_OK;
}