GF_Err stbl_GetSampleDTS_and_Duration(GF_TimeToSampleBox *stts, u32 SampleNumber, u64 *DTS, u32 *duration)
{
	u32 i, j, count;
	GF_SttsEntry *ent;

	(*DTS) = 0;
	if (duration) {
		*duration = 0;
	}
	if (!stts || !SampleNumber) return GF_BAD_PARAM;

	ent = NULL;
	//use our cache
	count = stts->nb_entries;
	if (stts->r_FirstSampleInEntry
	        && (stts->r_FirstSampleInEntry <= SampleNumber)
	        //this is for read/write access
	        && (stts->r_currentEntryIndex < count) ) {

		i = stts->r_currentEntryIndex;
	} else {
		i = stts->r_currentEntryIndex = 0;
		stts->r_FirstSampleInEntry = 1;
		stts->r_CurrentDTS = 0;
	}

	for (; i < count; i++) {
		ent = &stts->entries[i];

		//in our entry
		if (ent->sampleCount + stts->r_FirstSampleInEntry >= 1 + SampleNumber) {
			j = SampleNumber - stts->r_FirstSampleInEntry;
			goto found;
		}

		//update our cache
		stts->r_CurrentDTS += (u64)ent->sampleCount * ent->sampleDelta;
		stts->r_currentEntryIndex += 1;
		stts->r_FirstSampleInEntry += ent->sampleCount;
	}
//	if (SampleNumber >= stts->r_FirstSampleInEntry + ent->sampleCount) return GF_BAD_PARAM;

	//no ent, this is really weird. Let's assume the DTS is then what is written in the table
	if (!ent || (i == count)) {
		(*DTS) = stts->r_CurrentDTS;
		if (duration) *duration = ent ? ent->sampleDelta : 0;
	}
	return GF_OK;

found:
	(*DTS) = stts->r_CurrentDTS + j * (u64) ent->sampleDelta;
	if (duration) *duration = ent->sampleDelta;
	return GF_OK;
}