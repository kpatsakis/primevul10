GF_Err gf_isom_change_mpeg4_description(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, const GF_ESD *newESD)
{
	GF_Err e;
	GF_ESD *esd;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;

	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;

	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	//duplicate our desc
	e = gf_odf_desc_copy((GF_Descriptor *)newESD, (GF_Descriptor **)&esd);
	if (e) return e;
	e = Track_SetStreamDescriptor(trak, StreamDescriptionIndex, entry->dataReferenceIndex, esd, NULL);
	if (e != GF_OK) {
		gf_odf_desc_del((GF_Descriptor *) esd);
	}
	return e;
}