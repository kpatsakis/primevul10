GF_Err gf_isom_remove_edit(GF_ISOFile *movie, u32 trackNumber, u32 seg_index)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_EdtsEntry *ent, *next_ent;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !seg_index) return GF_BAD_PARAM;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (!trak->editBox || !trak->editBox->editList) return GF_OK;
	if (gf_list_count(trak->editBox->editList->entryList)<=1) return gf_isom_remove_edits(movie, trackNumber);

	ent = (GF_EdtsEntry*) gf_list_get(trak->editBox->editList->entryList, seg_index-1);
	gf_list_rem(trak->editBox->editList->entryList, seg_index-1);
	next_ent = (GF_EdtsEntry *)gf_list_get(trak->editBox->editList->entryList, seg_index-1);
	if (next_ent) next_ent->segmentDuration += ent->segmentDuration;
	gf_free(ent);
	return SetTrackDuration(trak);
}