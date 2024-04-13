GF_Err gf_isom_set_composition_offset_mode(GF_ISOFile *file, u32 track, Bool use_negative_offsets)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_CompositionOffsetBox *ctts;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;

	ctts = trak->Media->information->sampleTable->CompositionOffset;
	if (!ctts) {
		if (!use_negative_offsets && trak->Media->information->sampleTable->CompositionToDecode) {
			gf_isom_box_del_parent(&trak->Media->information->sampleTable->child_boxes, (GF_Box *)trak->Media->information->sampleTable->CompositionToDecode);
			trak->Media->information->sampleTable->CompositionToDecode = NULL;
		}
		return GF_OK;
	}

	if (use_negative_offsets) {
		return gf_isom_set_ctts_v1(file, track, 0);
	} else {
		if (ctts->version==0) return GF_OK;
		return gf_isom_set_ctts_v0(file, trak);
	}
}