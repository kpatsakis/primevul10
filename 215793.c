GF_Err stbl_RemoveDTS(GF_SampleTableBox *stbl, u32 sampleNumber, u32 nb_samples, u32 LastAUDefDuration)
{
	GF_SttsEntry *ent;
	GF_TimeToSampleBox *stts;

	if ((nb_samples>1) && (sampleNumber>1)) return GF_BAD_PARAM;

	stts = stbl->TimeToSample;

	//we're removing the only sample: empty the sample table
	if (stbl->SampleSize->sampleCount == 1) {
		stts->nb_entries = 0;
		stts->r_FirstSampleInEntry = stts->r_currentEntryIndex = 0;
		stts->r_CurrentDTS = 0;
		return GF_OK;
	}
	//we're removing the last sample
	if ((nb_samples==1) && (sampleNumber == stbl->SampleSize->sampleCount)) {
		ent = &stts->entries[stts->nb_entries-1];
		ent->sampleCount--;
		if (!ent->sampleCount) stts->nb_entries--;
	} else {
		u64 *DTSs, curDTS;
		u32 i, j, k, sampNum;
		u32 tot_samples, nb_written=0;
		//unpack the DTSs...
		DTSs = (u64*)gf_malloc(sizeof(u64) * (stbl->SampleSize->sampleCount - 1));
		if (!DTSs) return GF_OUT_OF_MEM;
		memset(DTSs, 0, sizeof(u64) * (stbl->SampleSize->sampleCount - 1) );

		curDTS = 0;
		sampNum = 0;
		ent = NULL;
		k=0;

		for (i=0; i<stts->nb_entries; i++) {
			ent = & stts->entries[i];
			for (j=0; j<ent->sampleCount; j++) {
				if (nb_samples==1) {
					if (sampNum == sampleNumber - 1) {
						k=1;
					} else {
						DTSs[sampNum-k] = curDTS;
					}
				} else if (sampNum >= nb_samples) {
					DTSs[sampNum - nb_samples] = curDTS;
					nb_written++;
				}
				curDTS += ent->sampleDelta;
				sampNum ++;
			}
		}

		if (nb_samples>1) {
			assert(sampNum == stbl->SampleSize->sampleCount);
			assert(nb_written + nb_samples == stbl->SampleSize->sampleCount);
		}
		j=0;

		if (nb_samples==1) {
			tot_samples = stbl->SampleSize->sampleCount - 1;
		} else {
			tot_samples = stbl->SampleSize->sampleCount - nb_samples;
		}
		if (tot_samples) {
			sampNum = 1;
			stts->nb_entries = 1;
			stts->entries[0].sampleCount = 1;
			if (stbl->SampleSize->sampleCount == 2) {
				stts->entries[0].sampleDelta = LastAUDefDuration;
			} else {
				stts->entries[0].sampleDelta = (u32) DTSs[1] /*- DTSs[0]*/;
			}
		} else {
			sampNum = 0;
			stts->nb_entries = 0;
		}

		for (i=1; i<tot_samples; i++) {
			if (i+1 == tot_samples) {
				//and by default, our last sample has the same delta as the prev
				stts->entries[j].sampleCount++;
				sampNum ++;
			} else if (DTSs[i+1] - DTSs[i] == stts->entries[j].sampleDelta) {
				stts->entries[j].sampleCount += 1;
				sampNum ++;
			} else {
				stts->nb_entries++;
				if (j+1==stts->alloc_size) {
					stts->alloc_size++;
					stts->entries = gf_realloc(stts->entries, sizeof(GF_SttsEntry) * stts->alloc_size);
					if (!stts->entries) return GF_OUT_OF_MEM;
				}
				j++;
				stts->entries[j].sampleCount = 1;
				stts->entries[j].sampleDelta = (u32) (DTSs[i+1] - DTSs[i]);
				assert(stts->entries[j].sampleDelta);
				sampNum ++;
			}
		}
		stts->w_LastDTS = tot_samples ? DTSs[tot_samples - 1] : 0;
		gf_free(DTSs);
		assert(sampNum == tot_samples);
		assert(sampNum + nb_samples == stbl->SampleSize->sampleCount);
	}

	//reset write the cache to the end
	stts->w_currentSampleNum = stbl->SampleSize->sampleCount - nb_samples;
	//reset read the cache to the beginning
	stts->r_FirstSampleInEntry = stts->r_currentEntryIndex = 0;
	stts->r_CurrentDTS = 0;
	return GF_OK;
}