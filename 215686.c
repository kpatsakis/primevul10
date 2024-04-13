GF_Err gf_isom_new_mpeg4_description(GF_ISOFile *movie,
                                     u32 trackNumber,
                                     const GF_ESD *esd,
                                     const char *URLname,
                                     const char *URNname,
                                     u32 *outDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_Err e;
	u32 dataRefIndex;
	GF_ESD *new_esd;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media ||
	        !esd || !esd->decoderConfig ||
	        !esd->slConfig) return GF_BAD_PARAM;

	//get or create the data ref
	e = Media_FindDataRef(trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
	if (e) return e;
	if (!dataRefIndex) {
		e = Media_CreateDataRef(movie, trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
		if (e) return e;
	}
	//duplicate our desc
	e = gf_odf_desc_copy((GF_Descriptor *)esd, (GF_Descriptor **)&new_esd);
	if (e) return e;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	e = Track_SetStreamDescriptor(trak, 0, dataRefIndex, new_esd, outDescriptionIndex);
	if (e) {
		gf_odf_desc_del((GF_Descriptor *)new_esd);
		return e;
	}
	return e;
}