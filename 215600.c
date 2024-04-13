GF_Err gf_isom_remove_sample_description(GF_ISOFile *movie, u32 trackNumber, u32 streamDescIndex)
{
	GF_TrackBox *trak;
	GF_Err e;
	GF_Box *entry;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !streamDescIndex) return GF_BAD_PARAM;
	entry = (GF_Box*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, streamDescIndex-1);
	if (!entry) return GF_BAD_PARAM;
	gf_list_rem(trak->Media->information->sampleTable->SampleDescription->child_boxes, streamDescIndex-1);
	gf_isom_box_del(entry);
	return GF_OK;
}