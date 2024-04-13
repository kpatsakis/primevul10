GF_Err gf_isom_reset_track_switch_parameter(GF_ISOFile *movie, u32 trackNumber, Bool reset_all_group)
{
	GF_TrackBox *trak;
	u32 alternateGroupID = 0;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Header->alternate_group) return GF_OK;

	alternateGroupID = trak->Header->alternate_group;
	if (reset_all_group) {
		u32 i=0;
		while (i< gf_isom_get_track_count(movie) ) {
			//locate first available ID
			GF_TrackBox *a_trak = gf_isom_get_track_from_file(movie, i+1);
			if (a_trak->Header->alternate_group == alternateGroupID) reset_tsel_box(a_trak);
			i++;
		}
	} else {
		reset_tsel_box(trak);
	}
	return GF_OK;
}