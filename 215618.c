Bool gf_isom_is_inplace_rewrite(GF_ISOFile *movie)
{
	if (!movie) return GF_FALSE;
	if (!movie->no_inplace_rewrite) {
		//things where added to the file, no inplace rewrite
		if (movie->editFileMap && gf_bs_get_size(movie->editFileMap->bs))
			movie->no_inplace_rewrite = GF_TRUE;
		//block redirect (used by mp4mx), no inplace rewrite
		else if (movie->on_block_out || !strcmp(movie->finalName, "_gpac_isobmff_redirect"))
			movie->no_inplace_rewrite = GF_TRUE;
		//stdout redirect, no inplace rewrite
		else if (!strcmp(movie->finalName, "std"))
			movie->no_inplace_rewrite = GF_TRUE;
		//new file, no inplace rewrite
		else if (!movie->fileName)
			movie->no_inplace_rewrite = GF_TRUE;
	}
	if (movie->no_inplace_rewrite) return GF_FALSE;

	return GF_TRUE;
}