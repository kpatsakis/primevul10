GF_Err gf_isom_set_audio_info(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 sampleRate, u32 nbChannels, u8 bitsPerSample, GF_AudioSampleEntryImportMode asemode)
{
	GF_Err e;
	u32 i, old_qtff_mode=GF_ISOM_AUDIO_QTFF_NONE;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_AudioSampleEntryBox*aud_entry;
	GF_SampleDescriptionBox *stsd;
	GF_Box *wave_box = NULL;
	GF_Box *gf_isom_audio_sample_get_audio_codec_cfg_box(GF_AudioSampleEntryBox *ptr);
	GF_Box *codec_ext = NULL;
#if 0
	GF_ChannelLayoutInfoBox *chan=NULL;
#endif
	GF_OriginalFormatBox *frma=NULL;
	GF_ChromaInfoBox *enda=NULL;
	GF_ESDBox *esds=NULL;
	GF_Box *terminator=NULL;
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

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_AUDIO) return GF_BAD_PARAM;
	aud_entry = (GF_AudioSampleEntryBox*) entry;

	if (entry->type==GF_ISOM_BOX_TYPE_MLPA) {
		aud_entry->samplerate_hi = sampleRate>>16;
		aud_entry->samplerate_lo = sampleRate & 0x0000FFFF;
	} else {
		aud_entry->samplerate_hi = sampleRate;
		aud_entry->samplerate_lo = 0;
	}
	aud_entry->bitspersample = bitsPerSample;

	switch (asemode) {
	case GF_IMPORT_AUDIO_SAMPLE_ENTRY_v0_2:
		stsd->version = 0;
		aud_entry->version = 0;
		aud_entry->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		aud_entry->channel_count = 2;
		break;
	case GF_IMPORT_AUDIO_SAMPLE_ENTRY_NOT_SET:
	case GF_IMPORT_AUDIO_SAMPLE_ENTRY_v0_BS:
		stsd->version = 0;
		aud_entry->version = 0;
		aud_entry->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		aud_entry->channel_count = nbChannels;
		break;
	case GF_IMPORT_AUDIO_SAMPLE_ENTRY_v1_MPEG:
		stsd->version = 1;
		aud_entry->version = 1;
		aud_entry->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		aud_entry->channel_count = nbChannels;
		break;
	case GF_IMPORT_AUDIO_SAMPLE_ENTRY_v1_QTFF:
		stsd->version = 0;
		aud_entry->version = 1;
		aud_entry->channel_count = nbChannels;
		old_qtff_mode = aud_entry->qtff_mode;
		if (aud_entry->qtff_mode != GF_ISOM_AUDIO_QTFF_ON_EXT_VALID)
			aud_entry->qtff_mode = GF_ISOM_AUDIO_QTFF_ON_NOEXT;
		break;
	}

	aud_entry->compression_id = 0;

	//check for wave+children and chan for QTFF or remove them for isobmff
	for (i=0; i<gf_list_count(aud_entry->child_boxes); i++) {
		GF_Box *b = gf_list_get(aud_entry->child_boxes, i);
		if ((b->type != GF_QT_BOX_TYPE_WAVE) && (b->type != GF_QT_BOX_TYPE_CHAN) ) continue;
		if (asemode==GF_IMPORT_AUDIO_SAMPLE_ENTRY_v1_QTFF) {
			if (b->type == GF_QT_BOX_TYPE_WAVE) wave_box = b;
#if 0
			else chan = (GF_ChannelLayoutInfoBox *)b;
#endif

		} else {
			gf_isom_box_del_parent(&aud_entry->child_boxes, b);
			i--;
		}
	}

	//TODO: insert channelLayout for ISOBMFF
	if (asemode!=GF_IMPORT_AUDIO_SAMPLE_ENTRY_v1_QTFF) return GF_OK;

	if (aud_entry->type==GF_ISOM_BOX_TYPE_MP4A)
		aud_entry->compression_id = -2;

	if (!aud_entry->child_boxes) aud_entry->child_boxes = gf_list_new();

#if 0
	if (!chan) {
		chan = (GF_ChannelLayoutInfoBox *) gf_isom_box_new_parent(&aud_entry->child_boxes, GF_QT_BOX_TYPE_CHAN);
	}
	//TODO, proper channel mapping
	chan->layout_tag = (nbChannels==2) ? 6750210 : 6553601;
#endif

	codec_ext = gf_isom_audio_sample_get_audio_codec_cfg_box((GF_AudioSampleEntryBox *)aud_entry);
	if (!codec_ext) return GF_OK;

	if (!wave_box) {
		wave_box = gf_isom_box_new_parent(&aud_entry->child_boxes, GF_QT_BOX_TYPE_WAVE);
	}

	for (i=0; i<gf_list_count(wave_box->child_boxes); i++) {
		GF_Box *b = gf_list_get(wave_box->child_boxes, i);
		switch (b->type) {
		case GF_QT_BOX_TYPE_ENDA:
			enda = (GF_ChromaInfoBox *)b;
			break;
		case GF_QT_BOX_TYPE_FRMA:
			frma = (GF_OriginalFormatBox *)b;
			break;
		case GF_ISOM_BOX_TYPE_ESDS:
			esds = (GF_ESDBox *)b;
			break;
		case GF_ISOM_BOX_TYPE_UNKNOWN:
			if ( ((GF_UnknownBox*)b)->original_4cc == 0)
				terminator = b;
			break;
		case 0:
			terminator = b;
			break;
		}
	}
	if (!wave_box->child_boxes) wave_box->child_boxes = gf_list_new();

	//do not use new_parent, we do this manually to ensure the order
	aud_entry->qtff_mode = old_qtff_mode ? old_qtff_mode : GF_ISOM_AUDIO_QTFF_ON_NOEXT;
	if (!frma) {
		frma = (GF_OriginalFormatBox *)gf_isom_box_new(GF_QT_BOX_TYPE_FRMA);
	} else {
		gf_list_del_item(wave_box->child_boxes, frma);
	}
	gf_list_add(wave_box->child_boxes, frma);

	if (esds) gf_list_del_item(wave_box->child_boxes, esds);
	if (!esds && (aud_entry->type==GF_ISOM_BOX_TYPE_MP4A) && ((GF_MPEGAudioSampleEntryBox*)aud_entry)->esd) {
		gf_list_del_item(entry->child_boxes, (GF_Box *) ((GF_MPEGAudioSampleEntryBox*)aud_entry)->esd);
		gf_list_add(wave_box->child_boxes, (GF_Box *) ((GF_MPEGAudioSampleEntryBox*)aud_entry)->esd);
	}

	if (!enda) {
		enda = (GF_ChromaInfoBox *)gf_isom_box_new(GF_QT_BOX_TYPE_ENDA);
	} else {
		gf_list_del_item(wave_box->child_boxes, enda);
	}
	enda->chroma=1;
	gf_list_add(wave_box->child_boxes, enda);

	if (!terminator) {
		terminator = gf_isom_box_new(0);
	} else {
		gf_list_del_item(wave_box->child_boxes, terminator);
	}
	gf_list_add(wave_box->child_boxes, terminator);

	if (aud_entry->type==GF_ISOM_BOX_TYPE_GNRA) {
		frma->data_format = ((GF_GenericAudioSampleEntryBox*) aud_entry)->EntryType;
	} else {
		frma->data_format = aud_entry->type;
	}

	return GF_OK;
}