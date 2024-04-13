GF_Err gf_isom_set_JPEG2000(GF_ISOFile *mov, Bool set_on)
{
	if (!mov) return GF_BAD_PARAM;
	mov->is_jp2 = set_on;
	return GF_OK;
}