GF_Err gf_isom_set_media_subtype(GF_ISOFile *movie, u32 trackNumber, u32 sampleDescriptionIndex, u32 new_type)
{
	GF_SampleEntryBox*entry;
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !sampleDescriptionIndex || !new_type) return GF_BAD_PARAM;

	entry = (GF_SampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescriptionIndex - 1);
	if (!entry) return GF_BAD_PARAM;
	entry->type = new_type;
	return GF_OK;
}