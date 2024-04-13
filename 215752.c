GF_Err gf_isom_set_interleave_time(GF_ISOFile *movie, u32 InterleaveTime)
{
	GF_Err e;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (!InterleaveTime || !movie->moov) return GF_OK;
	movie->interleavingTime = InterleaveTime;
	return GF_OK;
}