GF_Err gf_isom_text_set_display_flags(GF_ISOFile *file, u32 track, u32 desc_index, u32 flags, GF_TextFlagsMode op_type)
{
	u32 i;
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;

	for (i=0; i < gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes); i++) {
		GF_Tx3gSampleEntryBox *txt;
		if (desc_index && (i+1 != desc_index)) continue;

		txt = (GF_Tx3gSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, i);
		if (txt->type != GF_ISOM_BOX_TYPE_TX3G) continue;

		switch (op_type) {
		case GF_ISOM_TEXT_FLAGS_TOGGLE:
			txt->displayFlags |= flags;
			break;
		case GF_ISOM_TEXT_FLAGS_UNTOGGLE:
			txt->displayFlags &= ~flags;
			break;
		default:
			txt->displayFlags = flags;
			break;
		}
	}
	return GF_OK;

}