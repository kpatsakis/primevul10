GF_Err gf_isom_set_storage_mode(GF_ISOFile *movie, GF_ISOStorageMode storageMode)
{
	GF_Err e;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	switch (storageMode) {
	case GF_ISOM_STORE_FLAT:
	case GF_ISOM_STORE_STREAMABLE:
	case GF_ISOM_STORE_INTERLEAVED:
	case GF_ISOM_STORE_DRIFT_INTERLEAVED:
	case GF_ISOM_STORE_TIGHT:
	case GF_ISOM_STORE_FASTSTART:
		movie->storageMode = storageMode;
		//specifying a storage mode disables inplace rewrite
		gf_isom_disable_inplace_rewrite(movie);
		return GF_OK;
	default:
		return GF_BAD_PARAM;
	}
}