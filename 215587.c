GF_Err gf_isom_set_last_sample_duration(GF_ISOFile *movie, u32 trackNumber, u32 duration)
{
	return gf_isom_set_last_sample_duration_internal(movie, trackNumber, duration, 0, 0);
}