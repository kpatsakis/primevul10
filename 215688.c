GF_Err gf_isom_shift_cts_offset(GF_ISOFile *the_file, u32 trackNumber, s32 offset_shift)
{
	u32 i;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->CompositionOffset) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->CompositionOffset->unpack_mode) return GF_BAD_PARAM;

	for (i=0; i<trak->Media->information->sampleTable->CompositionOffset->nb_entries; i++) {
		/*we're in unpack mode: one entry per sample*/
		trak->Media->information->sampleTable->CompositionOffset->entries[i].decodingOffset -= offset_shift;
	}
	return GF_OK;
}