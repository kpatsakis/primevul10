GF_Err gf_isom_set_ctts_v1(GF_ISOFile *file, u32 track, u32 ctts_shift)
{
	u32 i, shift;
	u64 duration;
	GF_CompositionOffsetBox *ctts;
	GF_CompositionToDecodeBox *cslg;
	s32 leastCTTS, greatestCTTS;
	GF_TrackBox *trak;
	GF_Err e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

 	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;

	ctts = trak->Media->information->sampleTable->CompositionOffset;
	shift = ctts->version ? ctts_shift : ctts->entries[0].decodingOffset;
	leastCTTS = GF_INT_MAX;
	greatestCTTS = 0;
	for (i=0; i<ctts->nb_entries; i++) {
		if (!ctts->version)
			ctts->entries[i].decodingOffset -= shift;

		if ((s32)ctts->entries[i].decodingOffset < leastCTTS)
			leastCTTS = ctts->entries[i].decodingOffset;
		if ((s32)ctts->entries[i].decodingOffset > greatestCTTS)
			greatestCTTS = ctts->entries[i].decodingOffset;
	}
	if (!ctts->version) {
		ctts->version = 1;
		//if we had edit lists, shift all media times by the given amount
		if (trak->editBox && trak->editBox->editList) {
			for (i=0; i<gf_list_count(trak->editBox->editList->entryList); i++) {
				GF_EdtsEntry *ent = (GF_EdtsEntry*)gf_list_get(trak->editBox->editList->entryList, i);
				//empty edit
				if (ent->mediaTime<0) continue;
				if (ent->mediaTime>=shift) ent->mediaTime -= shift;
				else ent->mediaTime = 0;
				//no offset and last entry, trash edit
				if (!ent->mediaTime && (gf_list_count(trak->editBox->editList->entryList)==1)) {
					gf_isom_box_del_parent(&trak->child_boxes, (GF_Box *)trak->editBox);
					trak->editBox = NULL;
					break;
				}
			}
			SetTrackDuration(trak);
		}
	}

	if (!trak->Media->information->sampleTable->CompositionToDecode) {
		trak->Media->information->sampleTable->CompositionToDecode = (GF_CompositionToDecodeBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_CSLG);
		if (!trak->Media->information->sampleTable->CompositionToDecode)
			return GF_OUT_OF_MEM;
	}

	cslg = trak->Media->information->sampleTable->CompositionToDecode;
	if (cslg) {
		cslg->compositionToDTSShift = shift;
		cslg->leastDecodeToDisplayDelta = leastCTTS;
		cslg->greatestDecodeToDisplayDelta = greatestCTTS;
		cslg->compositionStartTime = 0;
		/*for our use case (first CTS set to 0), the composition end time is the media duration if it fits on 32 bits*/
		duration = gf_isom_get_media_duration(file, track);
		cslg->compositionEndTime = (duration<0x7FFFFFFF) ? (s32) duration : 0;
	}

	gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_ISO4, GF_TRUE);
	return GF_OK;
}