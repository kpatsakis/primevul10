static GF_Err gf_isom_set_last_sample_duration_internal(GF_ISOFile *movie, u32 trackNumber, u64 dur_num, u32 dur_den, u32 mode)
{
	GF_TrackBox *trak;
	GF_SttsEntry *ent;
	GF_TimeToSampleBox *stts;
	u64 mdur;
	u32 duration;
	GF_Err e;
	Bool is_patch = GF_FALSE;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (mode==0) {
		duration = (u32) dur_num;
	} else if (mode==1) {
		duration = (u32) dur_num;
		if (dur_den) {
			duration *= trak->Media->mediaHeader->timeScale;
			duration /= dur_den;
		}
	} else {
		is_patch = GF_TRUE;
	}
	mdur = trak->Media->mediaHeader->duration;
	stts = trak->Media->information->sampleTable->TimeToSample;
	if (!stts->nb_entries) return GF_BAD_PARAM;

	if (is_patch) {
		u32 i, avg_dur, nb_samp=0;
		u64 cum_dur=0;
		for (i=0; i<stts->nb_entries; i++) {
			ent = (GF_SttsEntry*) &stts->entries[i];
			cum_dur += ent->sampleCount*ent->sampleDelta;
			nb_samp += ent->sampleCount;
		}
		if (cum_dur <= dur_num || !nb_samp) return GF_OK;
		avg_dur = (u32) (dur_num / nb_samp);

		for (i=0; i<stts->nb_entries; i++) {
			ent = (GF_SttsEntry*) &stts->entries[i];
			ent->sampleDelta = avg_dur;
		}
		stts->w_LastDTS = dur_num - avg_dur;
		return GF_OK;
	}
	//get the last entry
	ent = (GF_SttsEntry*) &stts->entries[stts->nb_entries-1];
	if ((mode==1) && !duration && !dur_den) {
		//same as previous, nothing to adjust
		if (ent->sampleCount>1) return GF_OK;
		if (stts->nb_entries==1) return GF_OK;
		duration = stts->entries[stts->nb_entries-2].sampleDelta;
	}

	mdur -= ent->sampleDelta;
	mdur += duration;

	//we only have one sample
	if (ent->sampleCount == 1) {
		ent->sampleDelta = (u32) duration;
		if (mode && (stts->nb_entries>1) && (stts->entries[stts->nb_entries-2].sampleDelta==duration)) {
			stts->entries[stts->nb_entries-2].sampleCount++;
			stts->nb_entries--;
			//and update the write cache
			stts->w_currentSampleNum = trak->Media->information->sampleTable->SampleSize->sampleCount;
		}
	} else {
		if (ent->sampleDelta == duration) return GF_OK;
		ent->sampleCount -= 1;

		if (stts->nb_entries==stts->alloc_size) {
			stts->alloc_size++;
			stts->entries = (GF_SttsEntry*)gf_realloc(stts->entries, sizeof(GF_SttsEntry)*stts->alloc_size);
			if (!stts->entries) return GF_OUT_OF_MEM;
		}
		stts->entries[stts->nb_entries].sampleCount = 1;
		stts->entries[stts->nb_entries].sampleDelta = (u32) duration;
		stts->nb_entries++;
		//and update the write cache
		stts->w_currentSampleNum = trak->Media->information->sampleTable->SampleSize->sampleCount;
	}
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	trak->Media->mediaHeader->duration = mdur;
	return SetTrackDuration(trak);
}