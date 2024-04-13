GF_Err CanAccessMovie(GF_ISOFile *movie, GF_ISOOpenMode Mode)
{
	if (!movie) return GF_BAD_PARAM;
	if (movie->openMode < Mode) return GF_ISOM_INVALID_MODE;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY) return GF_ISOM_INVALID_MODE;
#endif
	return GF_OK;
}