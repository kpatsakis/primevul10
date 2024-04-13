GF_Err gf_isom_set_sample_group_in_traf(GF_ISOFile *file)
{
	GF_Err e;
	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	file->sample_groups_in_traf = GF_TRUE;
	return GF_OK;
}