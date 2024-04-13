GF_Err gf_isom_set_track_group(GF_ISOFile *file, u32 track_number, u32 track_group_id, u32 group_type, Bool do_add)
{
	u32 i, j;
	GF_TrackGroupTypeBox *trgt;
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track_number);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->groups) trak->groups = (GF_TrackGroupBox*) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TRGR);
	if (!trak->groups) return GF_OUT_OF_MEM;

	for (j=0; j<gf_list_count(file->moov->trackList); j++) {
		GF_TrackBox *a_trak = gf_list_get(file->moov->trackList, j);
		if (!a_trak->groups) continue;

		for (i=0; i<gf_list_count(a_trak->groups->groups); i++) {
			trgt = gf_list_get(a_trak->groups->groups, i);

			if (trgt->track_group_id==track_group_id) {
				if (trgt->group_type != group_type) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("A track with same group ID is already defined for different group type %s\n", gf_4cc_to_str(trgt->group_type) ));
					return GF_BAD_PARAM;
				}
				if (a_trak==trak) {
					if (!do_add) {
						gf_list_rem(trak->groups->groups, i);
						gf_isom_box_del_parent(&trak->groups->child_boxes, (GF_Box *)trgt);
					}
					return GF_OK;
				}
			}
		}
	}
	//not found, add new group
	trgt = (GF_TrackGroupTypeBox*) gf_isom_box_new_parent(&trak->groups->child_boxes, GF_ISOM_BOX_TYPE_TRGT);
	if (!trgt) return GF_OUT_OF_MEM;
	trgt->track_group_id = track_group_id;
	trgt->group_type = group_type;
	return gf_list_add(trak->groups->groups, trgt);
}