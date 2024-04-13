GF_Err gf_isom_fragment_set_sample_roll_group(GF_ISOFile *movie, GF_ISOTrackID trackID, u32 sample_number_in_frag, GF_ISOSampleRollType roll_type, s16 roll_distance)
{
	u32 grp_type = (roll_type>=GF_ISOM_SAMPLE_PREROLL) ? GF_ISOM_SAMPLE_GROUP_PROL : GF_ISOM_SAMPLE_GROUP_ROLL;
	if (roll_type==GF_ISOM_SAMPLE_PREROLL_NONE)
		roll_type = 0;

	return gf_isom_set_sample_group_info(movie, 0, trackID, sample_number_in_frag, grp_type, 0, &roll_distance, roll_type ? sg_roll_create_entry : NULL, roll_type ? sg_roll_compare_entry : NULL);
}