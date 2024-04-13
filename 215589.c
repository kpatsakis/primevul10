GF_Err gf_isom_clone_sample_descriptions(GF_ISOFile *the_file, u32 trackNumber, GF_ISOFile *orig_file, u32 orig_track, Bool reset_existing)
{
	u32 i;
	GF_TrackBox *dst_trak, *src_trak;
	GF_Err e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	dst_trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!dst_trak || !dst_trak->Media) return GF_BAD_PARAM;
	src_trak = gf_isom_get_track_from_file(orig_file, orig_track);
	if (!src_trak || !src_trak->Media) return GF_BAD_PARAM;

	if (reset_existing) {
		gf_isom_box_array_del(dst_trak->Media->information->sampleTable->SampleDescription->child_boxes);
		dst_trak->Media->information->sampleTable->SampleDescription->child_boxes = gf_list_new();
	}

	for (i=0; i<gf_list_count(src_trak->Media->information->sampleTable->SampleDescription->child_boxes); i++) {
		u32 outDesc;
		e = gf_isom_clone_sample_description(the_file, trackNumber, orig_file, orig_track, i+1, NULL, NULL, &outDesc);
		if (e) break;
	}
	return e;
}