GF_Err gf_isom_append_edit(GF_ISOFile *movie, u32 trackNumber, u64 EditDuration, u64 MediaTime, GF_ISOEditType EditMode)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_EdtsEntry *ent;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (!trak->editBox) {
		GF_EditBox *edts = (GF_EditBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_EDTS);
		if (!edts) return GF_OUT_OF_MEM;
		trak_on_child_box((GF_Box*)trak, (GF_Box *)edts, GF_FALSE);
		assert(trak->editBox);
	}
	if (!trak->editBox->editList) {
		GF_EditListBox *elst = (GF_EditListBox *) gf_isom_box_new_parent(&trak->editBox->child_boxes, GF_ISOM_BOX_TYPE_ELST);
		if (!elst) return GF_OUT_OF_MEM;
		edts_on_child_box((GF_Box*)trak->editBox, (GF_Box *)elst, GF_FALSE);
		assert(trak->editBox->editList);
	}
	ent = (GF_EdtsEntry *)gf_malloc(sizeof(GF_EdtsEntry));
	if (!ent) return GF_OUT_OF_MEM;

	ent->segmentDuration = EditDuration;
	switch (EditMode) {
	case GF_ISOM_EDIT_EMPTY:
		ent->mediaRate = 0x10000;
		ent->mediaTime = -1;
		break;
	case GF_ISOM_EDIT_DWELL:
		ent->mediaRate = 0;
		ent->mediaTime = MediaTime;
		break;
	default:
		ent->mediaRate = 0x10000;
		ent->mediaTime = MediaTime;
		break;
	}
	gf_list_add(trak->editBox->editList->entryList, ent);
	return SetTrackDuration(trak);
}