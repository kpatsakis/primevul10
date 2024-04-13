GF_Err gf_isom_set_nalu_length_field(GF_ISOFile *file, u32 track, u32 StreamDescriptionIndex, u32 nalu_size_length)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_MPEGVisualSampleEntryBox *ve;
	GF_SampleDescriptionBox *stsd;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd || !StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return GF_BAD_PARAM;
	}

	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (!entry || ! gf_isom_is_nalu_based_entry(trak->Media, entry)) {
		return GF_BAD_PARAM;
	}

	ve = (GF_MPEGVisualSampleEntryBox*)entry;
	if (ve->avc_config) ve->avc_config->config->nal_unit_size = nalu_size_length;
	if (ve->svc_config) ve->svc_config->config->nal_unit_size = nalu_size_length;
	if (ve->hevc_config) ve->hevc_config->config->nal_unit_size = nalu_size_length;
	if (ve->lhvc_config) ve->lhvc_config->config->nal_unit_size = nalu_size_length;
	return GF_OK;
}