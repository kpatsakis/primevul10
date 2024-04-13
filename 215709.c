GF_Err gf_isom_purge_track_reference(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	GF_TrackReferenceTypeBox *ref;
	u32 i=0;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	//no tref, nothing to remove
	if (!trak->References) return GF_OK;

	while ((ref = gf_list_enum(trak->References->child_boxes, &i))) {
		u32 k;
		if (!ref->reference_type) continue;

		for (k=0; k<ref->trackIDCount; k++) {
			u32 tk = gf_isom_get_track_by_id(the_file, ref->trackIDs[k]);
			if (!tk) {
				memmove(&ref->trackIDs[k], &ref->trackIDs[k+1], ref->trackIDCount-k-1);
				k--;
				ref->trackIDCount--;
			}
		}
		if (!ref->trackIDCount) {
			i--;
			gf_isom_box_del_parent(&trak->References->child_boxes, (GF_Box *) ref);
		}
	}
	if (!trak->References->child_boxes || !gf_list_count(trak->References->child_boxes)) {
		gf_isom_box_del_parent(&trak->child_boxes, (GF_Box *) trak->References);
		trak->References = NULL;
	}
	return GF_OK;
}