void gf_isom_disable_inplace_rewrite(GF_ISOFile *movie)
{
	if (movie)
		movie->no_inplace_rewrite = GF_TRUE;
}