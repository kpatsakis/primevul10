GF_Err gf_isom_set_clean_aperture(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 cleanApertureWidthN, u32 cleanApertureWidthD, u32 cleanApertureHeightN, u32 cleanApertureHeightD, u32 horizOffN, u32 horizOffD, u32 vertOffN, u32 vertOffD)
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
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	GF_CleanApertureBox *clap = (GF_CleanApertureBox *)gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_CLAP);
	if (!cleanApertureHeightD || !cleanApertureWidthD || !horizOffD || !vertOffD) {
		if (clap) gf_isom_box_del_parent(&entry->child_boxes, (GF_Box*)clap);
		return GF_OK;
	}
	if (!clap) {
		clap = (GF_CleanApertureBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_CLAP);
		if (!clap) return GF_OUT_OF_MEM;
	}

	clap->cleanApertureWidthN = cleanApertureWidthN;
	clap->cleanApertureWidthD = cleanApertureWidthD;
	clap->cleanApertureHeightN = cleanApertureHeightN;
	clap->cleanApertureHeightD = cleanApertureHeightD;
	clap->horizOffN = horizOffN;
	clap->horizOffD = horizOffD;
	clap->vertOffN = vertOffN;
	clap->vertOffD = vertOffD;
	return GF_OK;
}