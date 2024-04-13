GF_Err gf_isom_set_pixel_aspect_ratio(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, s32 hSpacing, s32 vSpacing, Bool force_par)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	GF_PixelAspectRatioBox *pasp = (GF_PixelAspectRatioBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_PASP);
	if (!hSpacing || !vSpacing || ((hSpacing == vSpacing) && !force_par))  {
		if (pasp) gf_isom_box_del_parent(&entry->child_boxes, (GF_Box *)pasp);
		return GF_OK;
	}
	if (!pasp) {
		pasp = (GF_PixelAspectRatioBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_PASP);
		if (!pasp) return GF_OUT_OF_MEM;
	}
	pasp->hSpacing = (u32) hSpacing;
	pasp->vSpacing = (u32) vSpacing;
	return GF_OK;
}