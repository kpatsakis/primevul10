GF_Err gf_isom_set_rvc_config(GF_ISOFile *movie, u32 track, u32 sampleDescriptionIndex, u16 rvc_predefined, char *mime, u8 *data, u32 size)
{
	GF_MPEGVisualSampleEntryBox *entry;
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, track);
	if (!trak) return GF_BAD_PARAM;


	entry = (GF_MPEGVisualSampleEntryBox *) gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescriptionIndex-1);
	if (!entry ) return GF_BAD_PARAM;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	GF_RVCConfigurationBox *rvcc = (GF_RVCConfigurationBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_RVCC);
	if (rvcc && rvcc->rvc_meta_idx) {
		gf_isom_remove_meta_item(movie, GF_FALSE, track, rvcc->rvc_meta_idx);
		rvcc->rvc_meta_idx = 0;
	}

	if (!rvcc) {
		rvcc = (GF_RVCConfigurationBox *) gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_RVCC);
		if (!rvcc) return GF_OUT_OF_MEM;
	}
	rvcc->predefined_rvc_config = rvc_predefined;
	if (!rvc_predefined) {
		u32 it_id=0;
		e = gf_isom_set_meta_type(movie, GF_FALSE, track, GF_META_TYPE_RVCI);
		if (e) return e;
		gf_isom_modify_alternate_brand(movie, GF_ISOM_BRAND_ISO2, GF_TRUE);
		e = gf_isom_add_meta_item_memory(movie, GF_FALSE, track, "rvcconfig.xml", &it_id, GF_META_ITEM_TYPE_MIME, mime, NULL, NULL, data, size, NULL);
		if (e) return e;
		rvcc->rvc_meta_idx = gf_isom_get_meta_item_count(movie, GF_FALSE, track);
	}
	return GF_OK;
}