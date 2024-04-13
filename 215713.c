static GF_Err gf_isom_set_ctts_v0(GF_ISOFile *file, GF_TrackBox *trak)
{
	u32 i;
	s32 shift;
	GF_CompositionOffsetBox *ctts;
	GF_CompositionToDecodeBox *cslg;

	ctts = trak->Media->information->sampleTable->CompositionOffset;

	if (!trak->Media->information->sampleTable->CompositionToDecode)
	{
		shift = 0;
		for (i=0; i<ctts->nb_entries; i++) {
			if (-ctts->entries[i].decodingOffset > shift)
				shift = -ctts->entries[i].decodingOffset;
		}
		if (shift > 0)
		{
			for (i=0; i<ctts->nb_entries; i++) {
				ctts->entries[i].decodingOffset += shift;
			}
		}
	}
	else
	{
		cslg = trak->Media->information->sampleTable->CompositionToDecode;
		shift = cslg->compositionToDTSShift;
		for (i=0; i<ctts->nb_entries; i++) {
			ctts->entries[i].decodingOffset += shift;
		}
		gf_isom_box_del_parent(&trak->Media->information->sampleTable->child_boxes, (GF_Box *)cslg);
		trak->Media->information->sampleTable->CompositionToDecode = NULL;
	}
	if (shift>0) {
		//no edits, insert one
		if (! trak->editBox) {
			u64 dur = trak->Media->mediaHeader->duration;
			dur *= file->moov->mvhd->timeScale;
			dur /= trak->Media->mediaHeader->timeScale;
			gf_isom_set_edit(file, gf_list_find(file->moov->trackList, trak)+1, 0, dur, shift, GF_ISOM_EDIT_NORMAL);
		} else {
			//otherwise shift media times in all entries
			for (i=0; i<gf_list_count(trak->editBox->editList->entryList); i++) {
				GF_EdtsEntry *ent = (GF_EdtsEntry*)gf_list_get(trak->editBox->editList->entryList, i);
				//empty edit
				if (ent->mediaTime<0) continue;
				ent->mediaTime += shift;
			}
			SetTrackDuration(trak);
		}
	}
	ctts->version = 0;
	gf_isom_modify_alternate_brand(file, GF_ISOM_BRAND_ISO4, GF_FALSE);
	return GF_OK;
}