GF_Err gf_isom_set_media_timescale(GF_ISOFile *the_file, u32 trackNumber, u32 newTS, u32 new_tsinc, u32 force_rescale_type)
{
	Double scale;
	u32 old_ts_inc=0;
	u32 old_timescale;
	GF_TrackBox *trak;
	GF_SampleTableBox *stbl;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->mediaHeader) return GF_BAD_PARAM;
	if ((trak->Media->mediaHeader->timeScale==newTS) && !new_tsinc)
		return GF_EOS;

	if (!newTS) newTS = trak->Media->mediaHeader->timeScale;
	scale = newTS;
	scale /= trak->Media->mediaHeader->timeScale;
	old_timescale = trak->Media->mediaHeader->timeScale;
	trak->Media->mediaHeader->timeScale = newTS;

	stbl = trak->Media->information->sampleTable;
	if (new_tsinc) {
		u32 i;
		if (!stbl->TimeToSample || !stbl->TimeToSample->nb_entries)
			return GF_BAD_PARAM;

		for (i=0; i<stbl->TimeToSample->nb_entries; i++) {
			if (!old_ts_inc)
				old_ts_inc = stbl->TimeToSample->entries[i].sampleDelta;
			else if (old_ts_inc<stbl->TimeToSample->entries[i].sampleDelta)
				old_ts_inc = stbl->TimeToSample->entries[i].sampleDelta;
		}

		if ((old_timescale==newTS) && (old_ts_inc==new_tsinc))
			return GF_EOS;

		if (!force_rescale_type)
			force_rescale_type = 1;
		else if (force_rescale_type==2) {
			gf_free(stbl->TimeToSample->entries);
			stbl->TimeToSample->alloc_size = 1;
			stbl->TimeToSample->nb_entries = 1;
			stbl->TimeToSample->entries = gf_malloc(sizeof(GF_SttsEntry));
			stbl->TimeToSample->entries[0].sampleDelta = new_tsinc;
			stbl->TimeToSample->entries[0].sampleCount = stbl->SampleSize->sampleCount;
		}


		for (i=0; i<stbl->TimeToSample->nb_entries; i++) {
			stbl->TimeToSample->entries[i].sampleDelta = new_tsinc;
		}

		if (stbl->CompositionOffset) {
			for (i=0; i<stbl->CompositionOffset->nb_entries; i++) {
				u32 old_offset = stbl->CompositionOffset->entries[i].decodingOffset;
				if (force_rescale_type==2) {
					u32 val = old_offset ;
					//get number of TS delta
					old_offset /= old_ts_inc;
					if (old_offset * old_ts_inc < val)
						old_offset++;
					old_offset *= new_tsinc;
				} else {
					old_offset *= new_tsinc;
					old_offset /= old_ts_inc;
				}
				stbl->CompositionOffset->entries[i].decodingOffset = old_offset;
			}
		}

#define RESCALE_TSVAL(_tsval) {\
			s64 val = ((s64) _tsval) * new_tsinc;\
			val /= old_ts_inc;\
			_tsval = (s32) val;\
		}

		if (stbl->CompositionToDecode) {
			RESCALE_TSVAL(stbl->CompositionToDecode->compositionEndTime)
			RESCALE_TSVAL(stbl->CompositionToDecode->compositionStartTime)
			RESCALE_TSVAL(stbl->CompositionToDecode->compositionToDTSShift)
			RESCALE_TSVAL(stbl->CompositionToDecode->greatestDecodeToDisplayDelta)
			RESCALE_TSVAL(stbl->CompositionToDecode->leastDecodeToDisplayDelta)
		}
		if (trak->editBox) {
			GF_EdtsEntry *ent;
			i=0;
			while ((ent = (GF_EdtsEntry*)gf_list_enum(trak->editBox->editList->entryList, &i))) {
				RESCALE_TSVAL(ent->mediaTime)
			}
		}
#undef RESCALE_TSVAL

		return SetTrackDuration(trak);
	}

	//rescale timings
	u32 i, k, idx, last_delta;
	u64 cur_dts;
	u64*DTSs = NULL;
	s64*CTSs = NULL;

	if (trak->editBox) {
		GF_EdtsEntry *ent;
		i=0;
		while ((ent = (GF_EdtsEntry*)gf_list_enum(trak->editBox->editList->entryList, &i))) {
			ent->mediaTime = (u32) (scale*ent->mediaTime);
		}
	}
	if (! stbl || !stbl->TimeToSample || !stbl->TimeToSample->nb_entries) {
		return SetTrackDuration(trak);
	}

	idx = 0;
	cur_dts = 0;
	//unpack the DTSs
	DTSs = (u64*)gf_malloc(sizeof(u64) * (stbl->SampleSize->sampleCount) );
	if (!DTSs) return GF_OUT_OF_MEM;

	CTSs = NULL;
	if (stbl->CompositionOffset) {
		CTSs = (s64*)gf_malloc(sizeof(u64) * (stbl->SampleSize->sampleCount) );
		if (!CTSs) return GF_OUT_OF_MEM;
	}

	for (i=0; i<stbl->TimeToSample->nb_entries; i++) {
		for (k=0; k<stbl->TimeToSample->entries[i].sampleCount; k++) {
			cur_dts += stbl->TimeToSample->entries[i].sampleDelta;
			DTSs[idx] = (u64) (cur_dts * scale);

			if (stbl->CompositionOffset) {
				s32 cts_o;
				stbl_GetSampleCTS(stbl->CompositionOffset, idx+1, &cts_o);
				CTSs[idx] = (s64) ( ((s64) cur_dts + cts_o) * scale);
			}
			idx++;
		}
	}
	last_delta = (u32) (stbl->TimeToSample->entries[stbl->TimeToSample->nb_entries-1].sampleDelta * scale);

	//repack DTS
	if (stbl->SampleSize->sampleCount) {
		stbl->TimeToSample->entries = gf_realloc(stbl->TimeToSample->entries, sizeof(GF_SttsEntry)*stbl->SampleSize->sampleCount);
		memset(stbl->TimeToSample->entries, 0, sizeof(GF_SttsEntry)*stbl->SampleSize->sampleCount);
		stbl->TimeToSample->entries[0].sampleDelta = (u32) DTSs[0];
		stbl->TimeToSample->entries[0].sampleCount = 1;
		idx=0;
		for (i=1; i< stbl->SampleSize->sampleCount - 1; i++) {
			if (DTSs[i+1] - DTSs[i] == stbl->TimeToSample->entries[idx].sampleDelta) {
				stbl->TimeToSample->entries[idx].sampleCount++;
			} else {
				idx++;
				stbl->TimeToSample->entries[idx].sampleDelta = (u32) ( DTSs[i+1] - DTSs[i] );
				stbl->TimeToSample->entries[idx].sampleCount=1;
			}
		}
		if (stbl->SampleSize->sampleCount > 1) {
			//add the sample delta for the last sample
			if (stbl->TimeToSample->entries[idx].sampleDelta == last_delta) {
				stbl->TimeToSample->entries[idx].sampleCount++;
			} else {
				idx++;
				stbl->TimeToSample->entries[idx].sampleDelta = last_delta;
				stbl->TimeToSample->entries[idx].sampleCount=1;
			}

			stbl->TimeToSample->nb_entries = idx+1;
			stbl->TimeToSample->entries = gf_realloc(stbl->TimeToSample->entries, sizeof(GF_SttsEntry)*stbl->TimeToSample->nb_entries);
		}
	}

	if (CTSs && stbl->SampleSize->sampleCount>0) {
		//repack CTS
		stbl->CompositionOffset->entries = gf_realloc(stbl->CompositionOffset->entries, sizeof(GF_DttsEntry)*stbl->SampleSize->sampleCount);
		memset(stbl->CompositionOffset->entries, 0, sizeof(GF_DttsEntry)*stbl->SampleSize->sampleCount);
		stbl->CompositionOffset->entries[0].decodingOffset = (s32) (CTSs[0] - DTSs[0]);
		stbl->CompositionOffset->entries[0].sampleCount = 1;
		idx=0;
		for (i=1; i< stbl->SampleSize->sampleCount; i++) {
			s32 cts_o = (s32) (CTSs[i] - DTSs[i]);
			if (cts_o == stbl->CompositionOffset->entries[idx].decodingOffset) {
				stbl->CompositionOffset->entries[idx].sampleCount++;
			} else {
				idx++;
				stbl->CompositionOffset->entries[idx].decodingOffset = cts_o;
				stbl->CompositionOffset->entries[idx].sampleCount=1;
			}
		}
		stbl->CompositionOffset->nb_entries = idx+1;
		stbl->CompositionOffset->entries = gf_realloc(stbl->CompositionOffset->entries, sizeof(GF_DttsEntry)*stbl->CompositionOffset->nb_entries);

		gf_free(CTSs);
	}
	gf_free(DTSs);

	if (stbl->CompositionToDecode) {
		stbl->CompositionToDecode->compositionEndTime = (s32) (stbl->CompositionToDecode->compositionEndTime * scale);
		stbl->CompositionToDecode->compositionStartTime = (s32)(stbl->CompositionToDecode->compositionStartTime * scale);
		stbl->CompositionToDecode->compositionToDTSShift = (s32)(stbl->CompositionToDecode->compositionToDTSShift * scale);
		stbl->CompositionToDecode->greatestDecodeToDisplayDelta = (s32)(stbl->CompositionToDecode->greatestDecodeToDisplayDelta * scale);
		stbl->CompositionToDecode->leastDecodeToDisplayDelta = (s32)(stbl->CompositionToDecode->leastDecodeToDisplayDelta * scale);
	}

	return SetTrackDuration(trak);
}