GF_Err gf_isom_update_edit_list_duration(GF_ISOFile *file, u32 track)
{
	u32 i;
	u64 trackDuration;
	GF_EdtsEntry *ent;
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;


	//the total duration is the media duration: adjust it in case...
	e = Media_SetDuration(trak);
	if (e) return e;

	//assert the timeScales are non-NULL
	if (!trak->moov->mvhd->timeScale || !trak->Media->mediaHeader->timeScale) return GF_ISOM_INVALID_FILE;
	trackDuration = (trak->Media->mediaHeader->duration * trak->moov->mvhd->timeScale) / trak->Media->mediaHeader->timeScale;

	//if we have an edit list, the duration is the sum of all the editList
	//entries' duration (always expressed in MovieTimeScale)
	if (trak->editBox && trak->editBox->editList) {
		u64 editListDuration = 0;
		GF_EditListBox *elst = trak->editBox->editList;
		i=0;
		while ((ent = (GF_EdtsEntry*)gf_list_enum(elst->entryList, &i))) {
			if (ent->segmentDuration > trackDuration)
				ent->segmentDuration = trackDuration;
			if (!ent->segmentDuration) {
				u64 diff;
				ent->segmentDuration = trackDuration;
				if (ent->mediaTime>0) {
					diff = ent->mediaTime;
					diff *= trak->moov->mvhd->timeScale;
					diff /= trak->Media->mediaHeader->timeScale;
					if (diff < ent->segmentDuration)
						ent->segmentDuration -= diff;
					/*
					else
						diff = 0;
					*/
				}
			}
			if ((ent->mediaTime>=0) && ((u64) ent->mediaTime>=trak->Media->mediaHeader->duration)) {
				ent->mediaTime = trak->Media->mediaHeader->duration;
			}
			editListDuration += ent->segmentDuration;
		}
		trackDuration = editListDuration;
	}
	if (!trackDuration) {
		trackDuration = (trak->Media->mediaHeader->duration * trak->moov->mvhd->timeScale) / trak->Media->mediaHeader->timeScale;
	}
	trak->Header->duration = trackDuration;

	return GF_OK;

}