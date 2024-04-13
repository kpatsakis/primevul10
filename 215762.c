GF_Err gf_isom_set_creation_time(GF_ISOFile *movie, u64 ctime, u64 mtime)
{
	if (!movie || !movie->moov) return GF_BAD_PARAM;
	movie->moov->mvhd->creationTime = ctime;
	movie->moov->mvhd->modificationTime = mtime;
	return GF_OK;
}