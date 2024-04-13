GF_Err gf_isom_patch_last_sample_duration(GF_ISOFile *movie, u32 trackNumber, u64 next_dts)
{
	return gf_isom_set_last_sample_duration_internal(movie, trackNumber, next_dts, 0, 2);
}