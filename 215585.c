GF_Err gf_isom_set_visual_bit_depth(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u16 bitDepth)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	switch (trak->Media->handler->handlerType) {
	case GF_ISOM_MEDIA_VISUAL:
	case GF_ISOM_MEDIA_PICT:
	case GF_ISOM_MEDIA_AUXV:
		break;
	default:
		return GF_OK;
	}

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) {
		return movie->LastError = GF_ISOM_INVALID_FILE;
	}
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_MPEGVisualSampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;
	entry->bit_depth = bitDepth;
	return GF_OK;
}