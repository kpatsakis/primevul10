GF_Err stbl_RemoveCTS(GF_SampleTableBox *stbl, u32 sampleNumber, u32 nb_samples)
{
	GF_CompositionOffsetBox *ctts = stbl->CompositionOffset;
	if (!ctts) return GF_OK;

	assert(ctts->unpack_mode);
	if ((nb_samples>1) && (sampleNumber>1)) return GF_BAD_PARAM;

	//last one...
	if (stbl->SampleSize->sampleCount == 1) {
		gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) ctts);
		stbl->CompositionOffset = NULL;
		return GF_OK;
	}

	//the number of entries is NOT ALWAYS the number of samples !
	//instead, use the cache
	//first case, we're removing a sample that was not added yet
	if (sampleNumber > ctts->w_LastSampleNumber) return GF_OK;

	if (nb_samples==1) {
		assert(ctts->nb_entries);
		memmove(&ctts->entries[sampleNumber-1], &ctts->entries[sampleNumber], sizeof(GF_DttsEntry)* (ctts->nb_entries-sampleNumber) );
		ctts->nb_entries--;
	} else {
		memmove(&ctts->entries[0], &ctts->entries[nb_samples], sizeof(GF_DttsEntry)* (ctts->nb_entries-nb_samples) );
		ctts->nb_entries -= nb_samples;
	}
	ctts->w_LastSampleNumber -= nb_samples;
	assert(ctts->w_LastSampleNumber >= ctts->nb_entries);

	return GF_OK;
}