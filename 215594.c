u32 gf_isom_new_track(GF_ISOFile *movie, GF_ISOTrackID trakID, u32 MediaType, u32 TimeScale)
{
	return gf_isom_new_track_from_template(movie, trakID, MediaType, TimeScale, NULL, 0, GF_FALSE);
}