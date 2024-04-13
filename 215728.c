GF_ISOTrackID gf_isom_get_last_created_track_id(GF_ISOFile *movie)
{
	return movie ? movie->last_created_track_id : 0;
}