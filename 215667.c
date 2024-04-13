GF_Err gf_isom_set_visual_info(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 Width, u32 Height)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) {
		return movie->LastError = GF_ISOM_INVALID_FILE;
	}
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	//valid for MPEG visual, JPG and 3GPP H263
	if (entry->internal_type == GF_ISOM_SAMPLE_ENTRY_VIDEO) {
		((GF_VisualSampleEntryBox*)entry)->Width = Width;
		((GF_VisualSampleEntryBox*)entry)->Height = Height;
		trak->Header->width = Width<<16;
		trak->Header->height = Height<<16;
		return GF_OK;
	} else if (trak->Media->handler->handlerType==GF_ISOM_MEDIA_SCENE) {
		trak->Header->width = Width<<16;
		trak->Header->height = Height<<16;
		return GF_OK;
	} else {
		return GF_BAD_PARAM;
	}
}