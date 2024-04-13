GF_Err gf_isom_change_sample_desc_index(GF_ISOFile *the_file, u32 trackNumber, u32 sample_number, u32 newSampleDescIndex)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !sample_number || !newSampleDescIndex) return GF_BAD_PARAM;
	if (!trak->is_unpacked) {
		unpack_track(trak);
	}
	if (!trak->Media->information->sampleTable->SampleToChunk) return GF_BAD_PARAM;
	if (trak->Media->information->sampleTable->SampleToChunk->nb_entries < sample_number) return GF_BAD_PARAM;
	trak->Media->information->sampleTable->SampleToChunk->entries[sample_number-1].sampleDescriptionIndex = newSampleDescIndex;
	return GF_OK;
}