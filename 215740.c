GF_Err gf_isom_set_high_dynamic_range_info(GF_ISOFile* movie, u32 trackNumber, u32 StreamDescriptionIndex, GF_MasteringDisplayColourVolumeInfo* mdcv, GF_ContentLightLevelInfo* clli)
{
	GF_Err e;
	GF_TrackBox* trak;
	GF_SampleEntryBox* entry;
	GF_SampleDescriptionBox* stsd;

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

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	GF_MasteringDisplayColourVolumeBox *mdcvb = (GF_MasteringDisplayColourVolumeBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_MDCV);
	if (!mdcvb) {
		mdcvb = (GF_MasteringDisplayColourVolumeBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_MDCV);
		if (!mdcvb) return GF_OUT_OF_MEM;
	}
	mdcvb->mdcv = *mdcv;

	/*clli*/
	GF_ContentLightLevelBox *cllib = (GF_ContentLightLevelBox *)gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_CLLI);
	if (!cllib) {
		cllib = (GF_ContentLightLevelBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_CLLI);
		if (!cllib) return GF_OUT_OF_MEM;
	}
	cllib->clli = *clli;
	return GF_OK;
}