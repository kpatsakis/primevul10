GF_Err gf_isom_set_track_reference(GF_ISOFile *the_file, u32 trackNumber, u32 referenceType, GF_ISOTrackID ReferencedTrackID)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_TrackReferenceBox *tref;
	GF_TrackReferenceTypeBox *dpnd;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	//no tref, create one
	tref = trak->References;
	if (!tref) {
		tref = (GF_TrackReferenceBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TREF);
		if (!tref) return GF_OUT_OF_MEM;
		e = trak_on_child_box((GF_Box*)trak, (GF_Box *) tref, GF_FALSE);
		if (e) return e;
	}
	//find a ref of the given type
	e = Track_FindRef(trak, referenceType, &dpnd);
	if (e) return e;

	if (!dpnd) {
		dpnd = (GF_TrackReferenceTypeBox *) gf_isom_box_new_parent(&tref->child_boxes, GF_ISOM_BOX_TYPE_REFT);
		if (!dpnd) return GF_OUT_OF_MEM;
		dpnd->reference_type = referenceType;
	}
	//add the ref
	return reftype_AddRefTrack(dpnd, ReferencedTrackID, NULL);
}