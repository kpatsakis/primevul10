GF_Err gf_isom_set_dolby_vision_profile(GF_ISOFile* movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 dv_profile)
{
	GF_Err e;
	GF_TrackBox* trak;
	GF_SampleEntryBox* entry;
	GF_SampleDescriptionBox* stsd;
	GF_DOVIConfigurationBox* dovi = NULL;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox*)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_OK;

	dovi = ((GF_MPEGVisualSampleEntryBox*)entry)->dovi_config;
	if (!dv_profile) {
		if (dovi) gf_isom_box_del((GF_Box*)dovi);
		((GF_MPEGVisualSampleEntryBox*)entry)->dovi_config = NULL;
		return GF_OK;
	}
	if (!dovi) {
		dovi = (GF_DOVIConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_DVCC);
		if (!dovi) return GF_OUT_OF_MEM;
		((GF_MPEGVisualSampleEntryBox*)entry)->dovi_config = dovi;
	}
	entry->type = GF_ISOM_BOX_TYPE_DVHE;
	dovi->DOVIConfig.dv_profile = dv_profile;
	return GF_OK;
}