GF_Err gf_isom_remove_cts_info(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_SampleTableBox *stbl;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stbl = trak->Media->information->sampleTable;
	if (!stbl->CompositionOffset) return GF_OK;

	gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *)stbl->CompositionOffset);
	stbl->CompositionOffset = NULL;
	return GF_OK;
}