GF_Err gf_isom_set_track_id(GF_ISOFile *movie, u32 trackNumber, GF_ISOTrackID trackID)
{
	GF_TrackReferenceTypeBox *ref;
	GF_TrackBox *trak, *a_trak;
	u32 i, j, k;

	if (!movie) return GF_BAD_PARAM;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (trak && (trak->Header->trackID==trackID)) return GF_OK;
	a_trak = gf_isom_get_track_from_id(movie->moov, trackID);
	if (!trak || a_trak) return GF_BAD_PARAM;

	if (movie->moov->mvhd->nextTrackID<=trackID)
		movie->moov->mvhd->nextTrackID = trackID;

	/*rewrite all dependencies*/
	i=0;
	while ((a_trak = (GF_TrackBox*)gf_list_enum(movie->moov->trackList, &i))) {
		if (!a_trak->References) continue;
		j=0;
		while ((ref = (GF_TrackReferenceTypeBox *)gf_list_enum(a_trak->References->child_boxes, &j))) {
			for (k=0; k<ref->trackIDCount; k++) {
				if (ref->trackIDs[k]==trak->Header->trackID) {
					ref->trackIDs[k] = trackID;
					break;
				}
			}
		}
	}

	/*and update IOD if any*/
	if (movie->moov->iods && movie->moov->iods->descriptor) {
		GF_ES_ID_Inc *inc;
		GF_IsomObjectDescriptor *od = (GF_IsomObjectDescriptor *)movie->moov->iods->descriptor;

		i=0;
		while ((inc = (GF_ES_ID_Inc*)gf_list_enum(od->ES_ID_IncDescriptors, &i))) {
			if (inc->trackID==trak->Header->trackID) inc->trackID = trackID;
		}
	}
	trak->Header->trackID = trackID;
	return GF_OK;
}