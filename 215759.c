GF_Err gf_isom_add_subsample(GF_ISOFile *movie, u32 track, u32 sampleNumber, u32 flags, u32 subSampleSize, u8 priority, u32 reserved, Bool discardable)
{
	u32 i, count;
	GF_SubSampleInformationBox *sub_samples;
	GF_TrackBox *trak;
	GF_Err e;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, track);
	if (!trak || !trak->Media || !trak->Media->information->sampleTable)
		return GF_BAD_PARAM;

	if (!trak->Media->information->sampleTable->sub_samples) {
		trak->Media->information->sampleTable->sub_samples=gf_list_new();
	}

	sub_samples = NULL;
	count = gf_list_count(trak->Media->information->sampleTable->sub_samples);
	for (i=0; i<count; i++) {
		sub_samples = gf_list_get(trak->Media->information->sampleTable->sub_samples, i);
		if (sub_samples->flags==flags) break;
		sub_samples = NULL;
	}
	if (!sub_samples) {
		sub_samples = (GF_SubSampleInformationBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_SUBS);
		if (!sub_samples) return GF_OUT_OF_MEM;
		gf_list_add(trak->Media->information->sampleTable->sub_samples, sub_samples);
		sub_samples->version = (subSampleSize>0xFFFF) ? 1 : 0;
		sub_samples->flags = flags;
	}
	return gf_isom_add_subsample_info(sub_samples, sampleNumber, subSampleSize, priority, reserved, discardable);
}