GF_Err gf_isom_set_last_sample_duration_ex(GF_ISOFile *movie, u32 trackNumber, u32 dur_num, u32 dur_den)
{
	return gf_isom_set_last_sample_duration_internal(movie, trackNumber, dur_num, dur_den, 1);
}