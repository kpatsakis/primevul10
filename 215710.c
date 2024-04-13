GF_Err gf_isom_set_cts_packing(GF_ISOFile *the_file, u32 trackNumber, Bool unpack)
{
	GF_Err e;
	GF_Err stbl_repackCTS(GF_CompositionOffsetBox *ctts);
	GF_Err stbl_unpackCTS(GF_SampleTableBox *stbl);
	GF_SampleTableBox *stbl;

	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stbl = trak->Media->information->sampleTable;
	if (unpack) {
		if (!stbl->CompositionOffset) {
			stbl->CompositionOffset = (GF_CompositionOffsetBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_CTTS);
			if (!stbl->CompositionOffset) return GF_OUT_OF_MEM;
		}
		e = stbl_unpackCTS(stbl);
	} else {
		if (!stbl->CompositionOffset) return GF_OK;
		e = stbl_repackCTS(stbl->CompositionOffset);
	}
	if (e) return e;
	return SetTrackDuration(trak);
}