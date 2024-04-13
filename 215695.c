GF_Err gf_isom_fragment_set_sample_rap_group(GF_ISOFile *movie, GF_ISOTrackID trackID, u32 sample_number_in_frag, Bool is_rap, u32 num_leading_samples)
{
	return gf_isom_set_sample_group_info(movie, 0, trackID, sample_number_in_frag, GF_ISOM_SAMPLE_GROUP_RAP, 0, &num_leading_samples, is_rap ? sg_rap_create_entry : NULL, is_rap ? sg_rap_compare_entry : NULL);
}