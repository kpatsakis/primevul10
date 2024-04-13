GF_Err gf_isom_set_sample_cenc_default_group(GF_ISOFile *movie, u32 track, u32 sample_number)
{
	return gf_isom_set_sample_group_info(movie, track, 0, sample_number, GF_ISOM_SAMPLE_GROUP_SEIG, 0, NULL, NULL, NULL);
}