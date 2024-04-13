GF_Err gf_isom_new_generic_sample_description(GF_ISOFile *movie, u32 trackNumber, const char *URLname, const char *URNname, GF_GenericSampleDescription *udesc, u32 *outDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_Err e;
	u32 dataRefIndex;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !udesc) return GF_BAD_PARAM;

	//get or create the data ref
	e = Media_FindDataRef(trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
	if (e) return e;
	if (!dataRefIndex) {
		e = Media_CreateDataRef(movie, trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
		if (e) return e;
	}
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	if (gf_isom_is_video_handler_type(trak->Media->handler->handlerType)) {
		GF_GenericVisualSampleEntryBox *entry;
		//create a new entry
		entry = (GF_GenericVisualSampleEntryBox*) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRV);
		if (!entry) return GF_OUT_OF_MEM;

		if (!udesc->codec_tag) {
			entry->EntryType = GF_ISOM_BOX_TYPE_UUID;
			memcpy(entry->uuid, udesc->UUID, sizeof(bin128));
		} else {
			entry->EntryType = udesc->codec_tag;
		}
		if (entry->EntryType == 0) {
			gf_isom_box_del((GF_Box *)entry);
			return GF_NOT_SUPPORTED;
		}

		entry->dataReferenceIndex = dataRefIndex;
		entry->vendor = udesc->vendor_code;
		entry->version = udesc->version;
		entry->revision = udesc->revision;
		entry->temporal_quality = udesc->temporal_quality;
		entry->spatial_quality = udesc->spatial_quality;
		entry->Width = udesc->width;
		entry->Height = udesc->height;
		strcpy(entry->compressor_name, udesc->compressor_name);
		entry->color_table_index = -1;
		entry->frames_per_sample = 1;
		entry->horiz_res = udesc->h_res ? udesc->h_res : 0x00480000;
		entry->vert_res = udesc->v_res ? udesc->v_res : 0x00480000;
		entry->bit_depth = udesc->depth ? udesc->depth : 0x18;
		if (udesc->extension_buf && udesc->extension_buf_size) {
			entry->data = (char*)gf_malloc(sizeof(char) * udesc->extension_buf_size);
			if (!entry->data) {
				gf_isom_box_del((GF_Box *) entry);
				return GF_OUT_OF_MEM;
			}
			memcpy(entry->data, udesc->extension_buf, udesc->extension_buf_size);
			entry->data_size = udesc->extension_buf_size;
		}
		e = gf_list_add(trak->Media->information->sampleTable->SampleDescription->child_boxes, entry);
	}
	else if (trak->Media->handler->handlerType==GF_ISOM_MEDIA_AUDIO) {
		GF_GenericAudioSampleEntryBox *gena;
		//create a new entry
		gena = (GF_GenericAudioSampleEntryBox*) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRA);
		if (!gena) return GF_OUT_OF_MEM;

		if (!udesc->codec_tag) {
			gena->EntryType = GF_ISOM_BOX_TYPE_UUID;
			memcpy(gena->uuid, udesc->UUID, sizeof(bin128));
		} else {
			gena->EntryType = udesc->codec_tag;
		}
		if (gena->EntryType == 0) {
			gf_isom_box_del((GF_Box *)gena);
			return GF_NOT_SUPPORTED;
		}

		gena->dataReferenceIndex = dataRefIndex;
		gena->vendor = udesc->vendor_code;
		gena->version = udesc->version;
		gena->revision = udesc->revision;
		gena->bitspersample = udesc->bits_per_sample ? udesc->bits_per_sample : 16;
		gena->channel_count = udesc->nb_channels ? udesc->nb_channels : 2;
		gena->samplerate_hi = udesc->samplerate;
		gena->samplerate_lo = 0;
		gena->qtff_mode = udesc->is_qtff ? GF_ISOM_AUDIO_QTFF_ON_NOEXT : GF_ISOM_AUDIO_QTFF_NONE;

		if (udesc->extension_buf && udesc->extension_buf_size) {
			gena->data = (char*)gf_malloc(sizeof(char) * udesc->extension_buf_size);
			if (!gena->data) {
				gf_isom_box_del((GF_Box *) gena);
				return GF_OUT_OF_MEM;
			}
			memcpy(gena->data, udesc->extension_buf, udesc->extension_buf_size);
			gena->data_size = udesc->extension_buf_size;
		}
		e = gf_list_add(trak->Media->information->sampleTable->SampleDescription->child_boxes, gena);
	}
	else {
		GF_GenericSampleEntryBox *genm;
		//create a new entry
		genm = (GF_GenericSampleEntryBox*) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRM);
		if (!genm) return GF_OUT_OF_MEM;

		if (!udesc->codec_tag) {
			genm->EntryType = GF_ISOM_BOX_TYPE_UUID;
			memcpy(genm->uuid, udesc->UUID, sizeof(bin128));
		} else {
			genm->EntryType = udesc->codec_tag;
		}
		if (genm->EntryType == 0) {
			gf_isom_box_del((GF_Box *)genm);
			return GF_NOT_SUPPORTED;
		}

		genm->dataReferenceIndex = dataRefIndex;
		if (udesc->extension_buf && udesc->extension_buf_size) {
			genm->data = (char*)gf_malloc(sizeof(char) * udesc->extension_buf_size);
			if (!genm->data) {
				gf_isom_box_del((GF_Box *) genm);
				return GF_OUT_OF_MEM;
			}
			memcpy(genm->data, udesc->extension_buf, udesc->extension_buf_size);
			genm->data_size = udesc->extension_buf_size;
		}
		e = gf_list_add(trak->Media->information->sampleTable->SampleDescription->child_boxes, genm);
	}
	*outDescriptionIndex = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
	return e;
}