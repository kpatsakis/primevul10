GF_Err gf_isom_set_audio_layout(GF_ISOFile *movie, u32 trackNumber, u32 sampleDescriptionIndex, GF_AudioChannelLayout *layout)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_AudioSampleEntryBox*aud_entry;
	GF_SampleDescriptionBox *stsd;
	GF_ChannelLayoutBox *chnl;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) {
		return movie->LastError = GF_ISOM_INVALID_FILE;
	}
	if (!sampleDescriptionIndex || sampleDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return movie->LastError = GF_BAD_PARAM;
	}
	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, sampleDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_AUDIO) return GF_BAD_PARAM;
	aud_entry = (GF_AudioSampleEntryBox*) entry;
	if (aud_entry->qtff_mode) {
		u32 sr = aud_entry->samplerate_hi;
		if (aud_entry->type==GF_ISOM_BOX_TYPE_MLPA) {
			sr <<= 16;
			sr |= aud_entry->samplerate_lo;
		}
		e = gf_isom_set_audio_info(movie, trackNumber, sampleDescriptionIndex, sr, aud_entry->channel_count, (u8) aud_entry->bitspersample, GF_IMPORT_AUDIO_SAMPLE_ENTRY_v1_MPEG);
		if (e) return e;
	}
	chnl = (GF_ChannelLayoutBox *) gf_isom_box_find_child(aud_entry->child_boxes, GF_ISOM_BOX_TYPE_CHNL);
	if (!chnl) {
		chnl = (GF_ChannelLayoutBox *)gf_isom_box_new_parent(&aud_entry->child_boxes, GF_ISOM_BOX_TYPE_CHNL);
		if (!chnl) return GF_OUT_OF_MEM;
	}
	aud_entry->channel_count = layout->channels_count;
	memcpy(&chnl->layout, layout, sizeof(GF_AudioChannelLayout));
	return GF_OK;
}