GF_Err gf_isom_set_image_sequence_coding_constraints(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, Bool remove, Bool all_ref_pics_intra, Bool intra_pred_used, u32 max_ref_per_pic)
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

	GF_CodingConstraintsBox*ccst = (GF_CodingConstraintsBox*) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_CCST);
	if (remove)  {
		if (ccst) gf_isom_box_del_parent(&entry->child_boxes, (GF_Box*)ccst);
		return GF_OK;
	}
	if (!ccst) {
		ccst = (GF_CodingConstraintsBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_CCST);
		if (!ccst) return GF_OUT_OF_MEM;
	}
	ccst->all_ref_pics_intra = all_ref_pics_intra;
	ccst->intra_pred_used = intra_pred_used;
	ccst->max_ref_per_pic = max_ref_per_pic;
	return GF_OK;
}