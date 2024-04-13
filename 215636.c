GF_Err gf_isom_update_video_sample_entry_fields(GF_ISOFile *file, u32 track, u32 stsd_idx, u16 revision, u32 vendor, u32 temporalQ, u32 spatialQ, u32 horiz_res, u32 vert_res, u16 frames_per_sample, const char *compressor_name, s16 color_table_index)
{

	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *vid_ent;

	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(file, track);
	if (!trak || !stsd_idx) return GF_BAD_PARAM;

	if (!trak->Media || !trak->Media->handler || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->SampleDescription)
		return GF_ISOM_INVALID_FILE;

	switch (trak->Media->handler->handlerType) {
	case GF_ISOM_MEDIA_VISUAL:
    case GF_ISOM_MEDIA_AUXV:
    case GF_ISOM_MEDIA_PICT:
    	break;
	default:
		return GF_BAD_PARAM;
	}
	vid_ent = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, stsd_idx-1);
	if (!vid_ent)
		return GF_BAD_PARAM;

	vid_ent->revision = revision;
	vid_ent->vendor = vendor;
	vid_ent->temporal_quality = temporalQ;
	vid_ent->spatial_quality = spatialQ;
	vid_ent->horiz_res = horiz_res;
	vid_ent->vert_res = vert_res;
	vid_ent->frames_per_sample = frames_per_sample;
	if (compressor_name)
		strncpy(vid_ent->compressor_name, compressor_name, 32);

	vid_ent->color_table_index = color_table_index;
	return GF_OK;
}