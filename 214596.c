GF_EXPORT
GF_Err gf_isom_get_visual_bit_depth(GF_ISOFile* movie, u32 trackNumber, u32 StreamDescriptionIndex, u16* bitDepth)
{
	GF_TrackBox* trak;
	GF_SampleEntryBox* entry;
	GF_SampleDescriptionBox* stsd;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) return movie->LastError = GF_BAD_PARAM;

	entry = (GF_SampleEntryBox*)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);

	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;

	//valid for MPEG visual, JPG and 3GPP H263
	if (entry->internal_type == GF_ISOM_SAMPLE_ENTRY_VIDEO) {
		*bitDepth = ((GF_VisualSampleEntryBox*)entry)->bit_depth;
	} else {
		return GF_BAD_PARAM;
	}
	return GF_OK;