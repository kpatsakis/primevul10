GF_Err gf_isom_rewrite_track_dependencies(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackReferenceTypeBox *ref;
	GF_TrackBox *trak, *a_trak;
	u32 i, k;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak)
		return GF_BAD_PARAM;
	if (!trak->References)
		return GF_OK;

	i=0;
	while ((ref = (GF_TrackReferenceTypeBox *)gf_list_enum(trak->References->child_boxes, &i))) {
		for (k=0; k < ref->trackIDCount; k++) {
			a_trak = gf_isom_get_track_from_original_id(movie->moov, ref->trackIDs[k], trak->originalFile);
			if (a_trak) {
				ref->trackIDs[k] = a_trak->Header->trackID;
			} else {
				a_trak = gf_isom_get_track_from_id(movie->moov, ref->trackIDs[k]);
				/*we should have a track with no original ID (not imported) - should we rewrite the dependency ?*/
				if (! a_trak || a_trak->originalID) return GF_BAD_PARAM;
			}
		}
	}

	return GF_OK;
}