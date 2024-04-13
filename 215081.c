GF_Err stbl_GetSampleCTS(GF_CompositionOffsetBox *ctts, u32 SampleNumber, s32 *CTSoffset)
{
	u32 i;

	(*CTSoffset) = 0;
	//test on SampleNumber is done before
	if (!ctts || !SampleNumber) return GF_BAD_PARAM;

	if (ctts->r_FirstSampleInEntry && (ctts->r_FirstSampleInEntry < SampleNumber) ) {
		i = ctts->r_currentEntryIndex;
	} else {
		ctts->r_FirstSampleInEntry = 1;
		ctts->r_currentEntryIndex = 0;
		i = 0;
	}
	for (; i< ctts->nb_entries; i++) {
		if (SampleNumber < ctts->r_FirstSampleInEntry + ctts->entries[i].sampleCount) break;
		//update our cache
		ctts->r_currentEntryIndex += 1;
		ctts->r_FirstSampleInEntry += ctts->entries[i].sampleCount;
	}
	//no ent, set everything to 0...
	if (i==ctts->nb_entries) return GF_OK;
	/*asked for a sample not in table - this means CTTS is 0 (that's due to out internal packing construction of CTTS)*/
	if (SampleNumber >= ctts->r_FirstSampleInEntry + ctts->entries[i].sampleCount) return GF_OK;
	(*CTSoffset) = ctts->entries[i].decodingOffset;
	return GF_OK;
}