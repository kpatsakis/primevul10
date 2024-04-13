GF_Err gf_isom_remove_sync_shadows(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	GF_SampleTableBox *stbl;

	if (movie->openMode == GF_ISOM_OPEN_READ) return GF_ISOM_INVALID_MODE;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stbl = trak->Media->information->sampleTable;
	if (stbl->ShadowSync) {
		gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) stbl->ShadowSync);
		stbl->ShadowSync = NULL;
	}
	return GF_OK;
}