GF_Err gf_isom_force_64bit_chunk_offset(GF_ISOFile *file, Bool set_on)
{
	if (!file) return GF_BAD_PARAM;
	file->force_co64 = set_on;
	return GF_OK;
}