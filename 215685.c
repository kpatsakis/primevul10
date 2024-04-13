GF_Err gf_isom_add_track_to_root_od(GF_ISOFile *movie, u32 trackNumber)
{
	GF_Err e;
	GF_ES_ID_Inc *inc;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	e = gf_isom_insert_moov(movie);
	if (e) return e;

	if (!movie->moov->iods) AddMovieIOD(movie->moov, 0);

	if (gf_isom_is_track_in_root_od(movie, trackNumber) == 1) return GF_OK;

	inc = (GF_ES_ID_Inc *) gf_odf_desc_new(GF_ODF_ESD_INC_TAG);
	inc->trackID = gf_isom_get_track_id(movie, trackNumber);
	if (!inc->trackID) {
		gf_odf_desc_del((GF_Descriptor *)inc);
		return movie->LastError;
	}
	if ( (movie->LastError = gf_isom_add_desc_to_root_od(movie, (GF_Descriptor *)inc) ) ) {
		return movie->LastError;
	}
	gf_odf_desc_del((GF_Descriptor *)inc);
	return GF_OK;
}