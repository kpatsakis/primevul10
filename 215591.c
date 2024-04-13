Bool gf_isom_is_same_sample_description(GF_ISOFile *f1, u32 tk1, u32 sdesc_index1, GF_ISOFile *f2, u32 tk2, u32 sdesc_index2)
{
	u32 i, count;
	GF_TrackBox *trak1, *trak2;
	GF_ESD *esd1, *esd2;
	Bool need_memcmp, ret;
	GF_Box *a, *b;

	/*get orig sample desc and clone it*/
	trak1 = gf_isom_get_track_from_file(f1, tk1);
	if (!trak1 || !trak1->Media) return GF_FALSE;
	trak2 = gf_isom_get_track_from_file(f2, tk2);
	if (!trak2 || !trak2->Media) return GF_FALSE;

	if (trak1->Media->handler->handlerType != trak2->Media->handler->handlerType) return GF_FALSE;
	count = gf_list_count(trak1->Media->information->sampleTable->SampleDescription->child_boxes);
	if (count != gf_list_count(trak2->Media->information->sampleTable->SampleDescription->child_boxes)) {
		if (!sdesc_index1 && !sdesc_index2) return GF_FALSE;
	}

	need_memcmp = GF_TRUE;
	for (i=0; i<count; i++) {
		GF_Box *ent1 = (GF_Box *)gf_list_get(trak1->Media->information->sampleTable->SampleDescription->child_boxes, i);
		GF_Box *ent2 = (GF_Box *)gf_list_get(trak2->Media->information->sampleTable->SampleDescription->child_boxes, i);

		if (sdesc_index1) ent1 = (GF_Box *)gf_list_get(trak1->Media->information->sampleTable->SampleDescription->child_boxes, sdesc_index1 - 1);
		if (sdesc_index2) ent2 = (GF_Box *)gf_list_get(trak2->Media->information->sampleTable->SampleDescription->child_boxes, sdesc_index2 - 1);

		if (!ent1 || !ent2) return GF_FALSE;
		if (ent1->type != ent2->type) return GF_FALSE;

		switch (ent1->type) {
		/*for MPEG-4 streams, only compare decSpecInfo (bitrate may not be the same but that's not an issue)*/
		case GF_ISOM_BOX_TYPE_MP4S:
		case GF_ISOM_BOX_TYPE_MP4A:
		case GF_ISOM_BOX_TYPE_MP4V:
		case GF_ISOM_BOX_TYPE_ENCA:
		case GF_ISOM_BOX_TYPE_ENCV:
		case GF_ISOM_BOX_TYPE_RESV:
		case GF_ISOM_BOX_TYPE_ENCS:
			Media_GetESD(trak1->Media, sdesc_index1 ? sdesc_index1 : i+1, &esd1, GF_TRUE);
			Media_GetESD(trak2->Media, sdesc_index2 ? sdesc_index2 : i+1, &esd2, GF_TRUE);
			if (!esd1 || !esd2) continue;
			need_memcmp = GF_FALSE;
			if (esd1->decoderConfig->streamType != esd2->decoderConfig->streamType) return GF_FALSE;
			if (esd1->decoderConfig->objectTypeIndication != esd2->decoderConfig->objectTypeIndication) return GF_FALSE;
			if (!esd1->decoderConfig->decoderSpecificInfo && esd2->decoderConfig->decoderSpecificInfo) return GF_FALSE;
			if (esd1->decoderConfig->decoderSpecificInfo && !esd2->decoderConfig->decoderSpecificInfo) return GF_FALSE;
			if (!esd1->decoderConfig->decoderSpecificInfo || !esd2->decoderConfig->decoderSpecificInfo) continue;
			if (memcmp(esd1->decoderConfig->decoderSpecificInfo->data, esd2->decoderConfig->decoderSpecificInfo->data, sizeof(char)*esd1->decoderConfig->decoderSpecificInfo->dataLength)!=0) return GF_FALSE;
			break;
		case GF_ISOM_BOX_TYPE_HVT1:
			return GF_TRUE;
		case GF_ISOM_BOX_TYPE_AVC1:
		case GF_ISOM_BOX_TYPE_AVC2:
		case GF_ISOM_BOX_TYPE_AVC3:
		case GF_ISOM_BOX_TYPE_AVC4:
		case GF_ISOM_BOX_TYPE_SVC1:
		case GF_ISOM_BOX_TYPE_MVC1:
		case GF_ISOM_BOX_TYPE_HVC1:
		case GF_ISOM_BOX_TYPE_HEV1:
		case GF_ISOM_BOX_TYPE_HVC2:
		case GF_ISOM_BOX_TYPE_HEV2:
		case GF_ISOM_BOX_TYPE_LHE1:
		case GF_ISOM_BOX_TYPE_LHV1:
		case GF_ISOM_BOX_TYPE_AV01:
		case GF_ISOM_BOX_TYPE_VVC1:
		case GF_ISOM_BOX_TYPE_VVI1:
		{
			GF_MPEGVisualSampleEntryBox *avc1 = (GF_MPEGVisualSampleEntryBox *)ent1;
			GF_MPEGVisualSampleEntryBox *avc2 = (GF_MPEGVisualSampleEntryBox *)ent2;

			if (avc1->hevc_config)
				a = (GF_Box *) avc1->hevc_config;
			else if (avc1->lhvc_config)
				a = (GF_Box *) avc1->lhvc_config;
			else if (avc1->svc_config)
				a = (GF_Box *) avc1->svc_config;
			else if (avc1->mvc_config)
				a = (GF_Box *) avc1->mvc_config;
			else if (avc1->av1_config)
				a = (GF_Box *)avc1->av1_config;
			else
				a = (GF_Box *) avc1->avc_config;

			if (avc2->hevc_config)
				b = (GF_Box *) avc2->hevc_config;
			else if (avc2->lhvc_config)
				b = (GF_Box *) avc2->lhvc_config;
			else if (avc2->svc_config)
				b = (GF_Box *) avc2->svc_config;
			else if (avc2->mvc_config)
				b = (GF_Box *) avc2->mvc_config;
			else if (avc2->av1_config)
				b = (GF_Box *)avc2->av1_config;
			else
				b = (GF_Box *) avc2->avc_config;

			return gf_isom_box_equal(a,b);
		}
		break;
		case GF_ISOM_BOX_TYPE_LSR1:
		{
			GF_LASeRSampleEntryBox *lsr1 = (GF_LASeRSampleEntryBox *)ent1;
			GF_LASeRSampleEntryBox *lsr2 = (GF_LASeRSampleEntryBox *)ent2;
			if (lsr1->lsr_config && lsr2->lsr_config
			        && lsr1->lsr_config->hdr && lsr2->lsr_config->hdr
			        && (lsr1->lsr_config->hdr_size==lsr2->lsr_config->hdr_size)
			        && !memcmp(lsr1->lsr_config->hdr, lsr2->lsr_config->hdr, lsr2->lsr_config->hdr_size)
			   ) {
				return GF_TRUE;
			}
			return GF_FALSE;
		}
		break;
#ifndef GPAC_DISABLE_VTT
		case GF_ISOM_BOX_TYPE_WVTT:
		{
			GF_WebVTTSampleEntryBox *wvtt1 = (GF_WebVTTSampleEntryBox *)ent1;
			GF_WebVTTSampleEntryBox *wvtt2 = (GF_WebVTTSampleEntryBox *)ent2;
			if (wvtt1->config && wvtt2->config &&
			        (wvtt1->config->string && wvtt2->config->string && !strcmp(wvtt1->config->string, wvtt2->config->string))) {
				return GF_TRUE;
			}
			return GF_FALSE;
		}
		break;
#endif
		case GF_ISOM_BOX_TYPE_STPP:
		{
			GF_MetaDataSampleEntryBox *stpp1 = (GF_MetaDataSampleEntryBox *)ent1;
			GF_MetaDataSampleEntryBox *stpp2 = (GF_MetaDataSampleEntryBox *)ent2;
			if (stpp1->xml_namespace && stpp2->xml_namespace && !strcmp(stpp1->xml_namespace, stpp2->xml_namespace)) {
				return GF_TRUE;
			}
			return GF_FALSE;
		}
		break;
		case GF_ISOM_BOX_TYPE_SBTT:
		{
			return GF_FALSE;
		}
		break;
		case GF_ISOM_BOX_TYPE_STXT:
		{
			GF_MetaDataSampleEntryBox *stxt1 = (GF_MetaDataSampleEntryBox *)ent1;
			GF_MetaDataSampleEntryBox *stxt2 = (GF_MetaDataSampleEntryBox *)ent2;
			if (stxt1->mime_type && stxt2->mime_type &&
			        ( (!stxt1->config && !stxt2->config) ||
			          (stxt1->config && stxt2->config && stxt1->config->config && stxt2->config->config &&
			           !strcmp(stxt1->config->config, stxt2->config->config)))) {
				return GF_TRUE;
			}
			return GF_FALSE;
		}
		case GF_ISOM_BOX_TYPE_MP3:
		case GF_QT_SUBTYPE_RAW_AUD:
		case GF_QT_SUBTYPE_TWOS:
		case GF_QT_SUBTYPE_SOWT:
		case GF_QT_SUBTYPE_FL32:
		case GF_QT_SUBTYPE_FL64:
		case GF_QT_SUBTYPE_IN24:
		case GF_QT_SUBTYPE_IN32:
		case GF_QT_SUBTYPE_ULAW:
		case GF_QT_SUBTYPE_ALAW:
		case GF_QT_SUBTYPE_ADPCM:
		case GF_QT_SUBTYPE_IMA_ADPCM:
		case GF_QT_SUBTYPE_DVCA:
		case GF_QT_SUBTYPE_QDMC:
		case GF_QT_SUBTYPE_QDMC2:
		case GF_QT_SUBTYPE_QCELP:
		case GF_QT_SUBTYPE_kMP3:
			return GF_TRUE;
		case GF_QT_SUBTYPE_APCH:
		case GF_QT_SUBTYPE_APCO:
		case GF_QT_SUBTYPE_APCN:
		case GF_QT_SUBTYPE_APCS:
		case GF_QT_SUBTYPE_AP4X:
		case GF_QT_SUBTYPE_AP4H:
		case GF_QT_SUBTYPE_RAW_VID:
		case GF_QT_SUBTYPE_YUYV:
		case GF_QT_SUBTYPE_UYVY:
		case GF_QT_SUBTYPE_YUV444:
		case GF_QT_SUBTYPE_YUVA444:
		case GF_QT_SUBTYPE_YUV422_10:
		case GF_QT_SUBTYPE_YUV444_10:
		case GF_QT_SUBTYPE_YUV422_16:
		case GF_QT_SUBTYPE_YUV420:
		case GF_QT_SUBTYPE_I420:
		case GF_QT_SUBTYPE_IYUV:
		case GF_QT_SUBTYPE_YV12:
		case GF_QT_SUBTYPE_YVYU:
		case GF_QT_SUBTYPE_RGBA:
		case GF_QT_SUBTYPE_ABGR:
			return GF_TRUE;
		}

		if (sdesc_index1 && sdesc_index2) break;
	}
	if (!need_memcmp) return GF_TRUE;
	a = (GF_Box *)trak1->Media->information->sampleTable->SampleDescription;
	b = (GF_Box *)trak2->Media->information->sampleTable->SampleDescription;
	//we ignore all bitrate boxes when comparing the box, disable their writing
	gf_isom_registry_disable(GF_ISOM_BOX_TYPE_BTRT, GF_TRUE);
	ret = gf_isom_box_equal(a,b);
	//re-enable btrt writing
	gf_isom_registry_disable(GF_ISOM_BOX_TYPE_BTRT, GF_FALSE);

	return ret;
}