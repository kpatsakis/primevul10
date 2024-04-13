GF_Err gf_isom_remove_edits(GF_ISOFile *movie, u32 trackNumber)
{
	GF_Err e;
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (!trak->editBox || !trak->editBox->editList) return GF_OK;

	while (gf_list_count(trak->editBox->editList->entryList)) {
		GF_EdtsEntry *ent = (GF_EdtsEntry*)gf_list_get(trak->editBox->editList->entryList, 0);
		gf_free(ent);
		e = gf_list_rem(trak->editBox->editList->entryList, 0);
		if (e) return e;
	}
	//then delete the GF_EditBox...
	gf_isom_box_del_parent(&trak->child_boxes, (GF_Box *)trak->editBox);
	trak->editBox = NULL;
	return SetTrackDuration(trak);
}