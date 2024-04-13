GF_Err gf_isom_set_visual_color_info(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 colour_type, u16 colour_primaries, u16 transfer_characteristics, u16 matrix_coefficients, Bool full_range_flag, u8 *icc_data, u32 icc_size)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;
	GF_ColourInformationBox *clr=NULL;
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

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_OK;

	clr = (GF_ColourInformationBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_COLR);
	if (!colour_type) {
		if (clr) gf_isom_box_del_parent(&entry->child_boxes, (GF_Box *)clr);
		return GF_OK;
	}
	if (!clr) {
		clr = (GF_ColourInformationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_COLR);
		if (!clr) return GF_OUT_OF_MEM;
	}
	clr->colour_type = colour_type;
	clr->colour_primaries = colour_primaries;
	clr->transfer_characteristics = transfer_characteristics;
	clr->matrix_coefficients = matrix_coefficients;
	clr->full_range_flag = full_range_flag;
	if (clr->opaque) gf_free(clr->opaque);
	clr->opaque = NULL;
	clr->opaque_size = 0;
	if ((colour_type==GF_ISOM_SUBTYPE_RICC) || (colour_type==GF_ISOM_SUBTYPE_PROF)) {
		clr->opaque_size = icc_data ? icc_size : 0;
		if (clr->opaque_size) {
			clr->opaque = gf_malloc(sizeof(char)*clr->opaque_size);
			if (!clr->opaque) return GF_OUT_OF_MEM;
			memcpy(clr->opaque, icc_data, sizeof(char)*clr->opaque_size);
		}
	}
	return GF_OK;
}