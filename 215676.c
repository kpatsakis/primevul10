static GF_Err CheckNoData(GF_ISOFile *movie)
{
	if (movie->openMode != GF_ISOM_OPEN_WRITE) return GF_OK;
	if (gf_bs_get_position(movie->editFileMap->bs)) return GF_BAD_PARAM;
	return GF_OK;
}