GF_Err gf_isom_reset_switch_parameters(GF_ISOFile *movie)
{
	u32 i=0;
	while (i< gf_isom_get_track_count(movie) ) {
		//locate first available ID
		GF_TrackBox *a_trak = gf_isom_get_track_from_file(movie, i+1);
		reset_tsel_box(a_trak);
		i++;
	}
	return GF_OK;
}