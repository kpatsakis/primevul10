GF_Err gf_isom_set_root_od_id(GF_ISOFile *movie, u32 OD_ID)
{
	GF_Err e;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	e = gf_isom_insert_moov(movie);
	if (e) return e;
	if (!movie->moov->iods) {
		e = AddMovieIOD(movie->moov, 0);
		if (e) return e;
	}

	switch (movie->moov->iods->descriptor->tag) {
	case GF_ODF_ISOM_OD_TAG:
		((GF_IsomObjectDescriptor *)movie->moov->iods->descriptor)->objectDescriptorID = OD_ID;
		break;
	case GF_ODF_ISOM_IOD_TAG:
		((GF_IsomInitialObjectDescriptor *)movie->moov->iods->descriptor)->objectDescriptorID = OD_ID;
		break;
	default:
		return GF_ISOM_INVALID_FILE;
	}
	return GF_OK;
}