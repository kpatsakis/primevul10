static GF_Err gf_isom_set_sample_group_info(GF_ISOFile *movie, u32 track, u32 trafID, u32 sample_number, u32 grouping_type, u32 grouping_type_parameter, void *udta, void *(*sg_create_entry)(void *udta), Bool (*sg_compare_entry)(void *udta, void *entry))
{
	GF_Err e;
	GF_TrackBox *trak=NULL;
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	GF_TrackFragmentBox *traf=NULL;
#endif
	if (!trafID && (movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY)) {
		trak = gf_isom_get_track_from_file(movie, track);
		if (!trak) return GF_BAD_PARAM;
		trafID = trak->Header->trackID;
	}

	if (trafID) {
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
		if (!movie->moof || !(movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY) )
			return GF_BAD_PARAM;

		traf = gf_isom_get_traf(movie, trafID);
#else
		return GF_NOT_SUPPORTED;
#endif

	} else if (track) {
		e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
		if (e) return e;

		trak = gf_isom_get_track_from_file(movie, track);
		if (!trak) return GF_BAD_PARAM;
	}

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	return gf_isom_set_sample_group_info_ex(trak ? trak->Media->information->sampleTable : NULL, traf, sample_number, grouping_type, grouping_type_parameter, udta, sg_create_entry, sg_compare_entry);
#else
	return gf_isom_set_sample_group_info_ex(trak->Media->information->sampleTable, sample_number, grouping_type, grouping_type_parameter, udta, sg_create_entry, sg_compare_entry);
#endif

}