GF_Err gf_isom_modify_edit(GF_ISOFile *movie, u32 trackNumber, u32 seg_index, u64 EditDuration, u64 MediaTime, GF_ISOEditType EditMode)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_EdtsEntry *ent;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !seg_index) return GF_BAD_PARAM;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (!trak->editBox || !trak->editBox->editList) return GF_OK;
	if (gf_list_count(trak->editBox->editList->entryList)<seg_index) return GF_BAD_PARAM;
	ent = (GF_EdtsEntry*) gf_list_get(trak->editBox->editList->entryList, seg_index-1);

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
	return SetTrackDuration(trak);
}