GF_Err gf_isom_remove_track_reference(GF_ISOFile *isom_file, u32 trackNumber, u32 ref_type)
{
	GF_TrackBox *trak;
	u32 i=0;
	GF_TrackReferenceTypeBox *ref;
	trak = gf_isom_get_track_from_file(isom_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (!trak->References) return GF_OK;
	while ((ref = gf_list_enum(trak->References->child_boxes, &i))) {
		if (ref->reference_type == ref_type) {
			gf_isom_box_del_parent(&trak->References->child_boxes, (GF_Box *)ref);
			break;
		}
	}
	return GF_OK;

}