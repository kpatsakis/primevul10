static GF_Err gf_isom_set_edit_internal(GF_ISOFile *movie, u32 trackNumber, u64 EditTime, u64 EditDuration, u64 MediaTime, u32 media_rate, GF_ISOEditType EditMode)
{
	GF_TrackBox *trak;
	GF_EditBox *edts;
	GF_EditListBox *elst;
	GF_EdtsEntry *ent, *newEnt;
	u32 i;
	GF_Err e;
	u64 startTime;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	edts = trak->editBox;
	if (! edts) {
		edts = (GF_EditBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_EDTS);
		if (!edts) return GF_OUT_OF_MEM;
		trak_on_child_box((GF_Box*)trak, (GF_Box *)edts, GF_FALSE);
	}
	elst = edts->editList;
	if (!elst) {
		elst = (GF_EditListBox *) gf_isom_box_new_parent(&edts->child_boxes, GF_ISOM_BOX_TYPE_ELST);
		if (!elst) return GF_OUT_OF_MEM;
		edts_on_child_box((GF_Box*)edts, (GF_Box *)elst, GF_FALSE);
	}

	startTime = 0;
	ent = NULL;
	//get the prev entry to this startTime if any
	i=0;
	while ((ent = (GF_EdtsEntry *)gf_list_enum(elst->entryList, &i))) {
		if ( (startTime <= EditTime) && (startTime + ent->segmentDuration > EditTime) )
			goto found;
		startTime += ent->segmentDuration;
	}

	//not found, add a new entry and adjust the prev one if any
	if (!ent) {
		newEnt = CreateEditEntry(EditDuration, MediaTime, EditMode);
		if (!newEnt) return GF_OUT_OF_MEM;
		if (EditMode==GF_ISOM_EDIT_NORMAL+1) {
			newEnt->mediaRate = media_rate;
		}
		gf_list_add(elst->entryList, newEnt);
		return SetTrackDuration(trak);
	}

	startTime -= ent->segmentDuration;

found:

	//if same time, we erase the current one...
	if (startTime == EditTime) {
		ent->segmentDuration = EditDuration;
		if (EditMode==GF_ISOM_EDIT_NORMAL+1) {
			ent->mediaRate = media_rate;
			ent->mediaTime = MediaTime;
		} else {
			switch (EditMode) {
			case GF_ISOM_EDIT_EMPTY:
				ent->mediaRate = 0x10000;
				ent->mediaTime = -1;
				break;
			case GF_ISOM_EDIT_DWELL:
				ent->mediaRate = 0;
				ent->mediaTime = MediaTime;
				break;
			default:
				ent->mediaRate = 0x10000;
				ent->mediaTime = MediaTime;
				break;
			}
		}
		return SetTrackDuration(trak);
	}

	//adjust so that the prev ent leads to EntryTime
	//Note: we don't change the next one as it is unknown to us in
	//a lot of case (the author's changes)
	ent->segmentDuration = EditTime - startTime;
	newEnt = CreateEditEntry(EditDuration, MediaTime, EditMode);
	if (!newEnt) return GF_OUT_OF_MEM;
	if (EditMode==GF_ISOM_EDIT_NORMAL+1) {
		newEnt->mediaRate = media_rate;
		newEnt->mediaTime = MediaTime;
	}
	//is it the last entry ???
	if (i >= gf_list_count(elst->entryList) - 1) {
		//add the new entry at the end
		gf_list_add(elst->entryList, newEnt);
		return SetTrackDuration(trak);
	} else {
		//insert after the current entry (which is i)
		gf_list_insert(elst->entryList, newEnt, i+1);
		return SetTrackDuration(trak);
	}
}