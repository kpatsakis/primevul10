GF_Err gf_isom_clone_track(GF_ISOFile *orig_file, u32 orig_track, GF_ISOFile *dest_file, GF_ISOTrackCloneFlags flags, u32 *dest_track)
{
	GF_TrackBox *trak, *new_tk;
	GF_BitStream *bs;
	u8 *data;
	const u8 *buffer;
	u32 data_size;
	u32 i, count;
	GF_Err e;
	GF_SampleTableBox *stbl, *stbl_temp;
	GF_SampleEncryptionBox *senc;

	e = CanAccessMovie(dest_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	e = gf_isom_insert_moov(dest_file);
	if (e) return e;

	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(orig_file, orig_track);
	if (!trak || !trak->Media) return GF_BAD_PARAM;

	stbl = trak->Media->information->sampleTable;
	stbl_temp = (GF_SampleTableBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STBL);
	if (!stbl_temp->child_boxes) stbl_temp->child_boxes = gf_list_new();

	trak->Media->information->sampleTable = stbl_temp;
	gf_list_add(trak->Media->information->child_boxes, stbl_temp);
	gf_list_del_item(trak->Media->information->child_boxes, stbl);

	if (!stbl_temp->child_boxes) stbl_temp->child_boxes = gf_list_new();

	/*clone sampleDescription table*/
	stbl_temp->SampleDescription = stbl->SampleDescription;
	gf_list_add(stbl_temp->child_boxes, stbl->SampleDescription);
	/*also clone sampleGroups description tables if any*/
	stbl_temp->sampleGroupsDescription = stbl->sampleGroupsDescription;
	count = gf_list_count(stbl->sampleGroupsDescription);
	for (i=0; i<count; i++) {
		GF_Box *b = gf_list_get(stbl->sampleGroupsDescription, i);
		gf_list_add(stbl_temp->child_boxes, b);
	}
	/*clone CompositionToDecode table, we may remove it later*/
	stbl_temp->CompositionToDecode = stbl->CompositionToDecode;
	gf_list_add(stbl_temp->child_boxes, stbl->CompositionToDecode);

	senc = trak->sample_encryption;
	if (senc) {
		assert(trak->child_boxes);
		gf_list_del_item(trak->child_boxes, senc);
		trak->sample_encryption = NULL;
	}

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	gf_isom_box_size( (GF_Box *) trak);
	gf_isom_box_write((GF_Box *) trak, bs);
	gf_bs_get_content(bs, &data, &data_size);
	gf_bs_del(bs);
	bs = gf_bs_new(data, data_size, GF_BITSTREAM_READ);
	if (flags & GF_ISOM_CLONE_TRACK_NO_QT)
		gf_bs_set_cookie(bs, GF_ISOM_BS_COOKIE_QT_CONV | GF_ISOM_BS_COOKIE_CLONE_TRACK);
	else
		gf_bs_set_cookie(bs, GF_ISOM_BS_COOKIE_CLONE_TRACK);

	e = gf_isom_box_parse((GF_Box **) &new_tk, bs);
	gf_bs_del(bs);
	gf_free(data);

	trak->Media->information->sampleTable = stbl;
	gf_list_del_item(trak->Media->information->child_boxes, stbl_temp);
	gf_list_add(trak->Media->information->child_boxes, stbl);

	if (senc) {
		trak->sample_encryption = senc;
		gf_list_add(trak->child_boxes, senc);
	}
	gf_list_del_item(stbl_temp->child_boxes, stbl_temp->SampleDescription);
	stbl_temp->SampleDescription = NULL;

	count = gf_list_count(stbl->sampleGroupsDescription);
	for (i=0; i<count; i++) {
		GF_Box *b = gf_list_get(stbl->sampleGroupsDescription, i);
		gf_list_del_item(stbl_temp->child_boxes, b);
	}
	stbl_temp->sampleGroupsDescription = NULL;

	gf_list_del_item(stbl_temp->child_boxes, stbl_temp->CompositionToDecode);
	stbl_temp->CompositionToDecode = NULL;
	gf_isom_box_del((GF_Box *)stbl_temp);

	if (e) {
		if (new_tk) gf_isom_box_del((GF_Box *)new_tk);
		return e;
	}

	gf_isom_disable_inplace_rewrite(dest_file);

	/*create default boxes*/
	stbl = new_tk->Media->information->sampleTable;
	stbl->ChunkOffset = gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STCO);
	if (!stbl->ChunkOffset) return GF_OUT_OF_MEM;
	stbl->SampleSize = (GF_SampleSizeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSZ);
	if (!stbl->SampleSize) return GF_OUT_OF_MEM;
	stbl->SampleToChunk = (GF_SampleToChunkBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSC);
	if (!stbl->SampleToChunk) return GF_OUT_OF_MEM;
	stbl->TimeToSample = (GF_TimeToSampleBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STTS);
	if (!stbl->TimeToSample) return GF_OUT_OF_MEM;

	/*check trackID validity before adding track*/
	if (gf_isom_get_track_by_id(dest_file, new_tk->Header->trackID)) {
		u32 ID = 1;
		while (1) {
			if (RequestTrack(dest_file->moov, ID)) break;
			ID += 1;
			if (ID == 0xFFFFFFFF) break;
		}
		new_tk->Header->trackID = ID;
	}
	if (!dest_file->moov->child_boxes) dest_file->moov->child_boxes = gf_list_new();
	gf_list_add(dest_file->moov->child_boxes, new_tk);
	moov_on_child_box((GF_Box*)dest_file->moov, (GF_Box *)new_tk, GF_FALSE);

	/*set originalID*/
	new_tk->originalID = trak->Header->trackID;
	/*set originalFile*/
	buffer = gf_isom_get_filename(orig_file);
	new_tk->originalFile = gf_crc_32(buffer, (u32) strlen(buffer));

	/*rewrite edit list segmentDuration to new movie timescale*/
	if (dest_file->moov->mvhd->timeScale != orig_file->moov->mvhd->timeScale) {
		Double ts_scale = dest_file->moov->mvhd->timeScale;
		ts_scale /= orig_file->moov->mvhd->timeScale;
		new_tk->Header->duration = (u64) (new_tk->Header->duration * ts_scale);
		if (new_tk->editBox && new_tk->editBox->editList) {
			count = gf_list_count(new_tk->editBox->editList->entryList);
			for (i=0; i<count; i++) {
				GF_EdtsEntry *ent = (GF_EdtsEntry *)gf_list_get(new_tk->editBox->editList->entryList, i);
				ent->segmentDuration = (u64) (ent->segmentDuration * ts_scale);
			}
		}
	}

	if (!new_tk->Media->information->dataInformation->dref) return GF_BAD_PARAM;

	/*reset data ref*/
	if (! (flags & GF_ISOM_CLONE_TRACK_KEEP_DREF) ) {
		GF_SampleEntryBox *entry;
		Bool use_alis = GF_FALSE;
		if (! (flags & GF_ISOM_CLONE_TRACK_NO_QT)) {
			GF_Box *b = gf_list_get(new_tk->Media->information->dataInformation->dref->child_boxes, 0);
			if (b && b->type==GF_QT_BOX_TYPE_ALIS)
				use_alis = GF_TRUE;
		}
		gf_isom_box_array_del(new_tk->Media->information->dataInformation->dref->child_boxes);
		new_tk->Media->information->dataInformation->dref->child_boxes = gf_list_new();
		/*update data ref*/
		entry = (GF_SampleEntryBox*)gf_list_get(new_tk->Media->information->sampleTable->SampleDescription->child_boxes, 0);
		if (entry) {
			u32 dref;
			Media_CreateDataRef(dest_file, new_tk->Media->information->dataInformation->dref, use_alis ?  "alis" : NULL, NULL, &dref);
			entry->dataReferenceIndex = dref;
		}
	} else {
		for (i=0; i<gf_list_count(new_tk->Media->information->dataInformation->dref->child_boxes); i++) {
			GF_DataEntryBox *dref_entry = (GF_DataEntryBox *)gf_list_get(new_tk->Media->information->dataInformation->dref->child_boxes, i);
			if (dref_entry->flags & 1) {
				dref_entry->flags &= ~1;
				e = Media_SetDrefURL((GF_DataEntryURLBox *)dref_entry, orig_file->fileName, dest_file->finalName);
				if (e) return e;
			}
		}
	}

	*dest_track = gf_list_count(dest_file->moov->trackList);

	if (dest_file->moov->mvhd->nextTrackID<= new_tk->Header->trackID)
		dest_file->moov->mvhd->nextTrackID = new_tk->Header->trackID+1;

	return GF_OK;
}