GF_Err gf_isom_add_sample_shadow(GF_ISOFile *movie, u32 trackNumber, GF_ISOSample *sample)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_ISOSample *prev;
	GF_SampleEntryBox *entry;
	u32 dataRefIndex;
	u64 data_offset;
	u32 descIndex;
	u32 sampleNum, prevSampleNum;
	GF_DataEntryURLBox *Dentry;
	Bool offset_times = GF_FALSE;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !sample) return GF_BAD_PARAM;

	e = FlushCaptureMode(movie);
	if (e) return e;

	e = unpack_track(trak);
	if (e) return e;

	e = stbl_findEntryForTime(trak->Media->information->sampleTable, sample->DTS, 0, &sampleNum, &prevSampleNum);
	if (e) return e;
	/*we need the EXACT match*/
	if (!sampleNum) return GF_BAD_PARAM;

	prev = gf_isom_get_sample_info(movie, trackNumber, sampleNum, &descIndex, NULL);
	if (!prev) return gf_isom_last_error(movie);
	/*for conformance*/
	if (sample->DTS==prev->DTS) offset_times = GF_TRUE;
	gf_isom_sample_del(&prev);

	e = Media_GetSampleDesc(trak->Media, descIndex, &entry, &dataRefIndex);
	if (e) return e;
	if (!entry || !dataRefIndex) return GF_BAD_PARAM;
	trak->Media->information->sampleTable->currentEntryIndex = descIndex;

	//get this dataRef and return false if not self contained
	Dentry = (GF_DataEntryURLBox*)gf_list_get(trak->Media->information->dataInformation->dref->child_boxes, dataRefIndex - 1);
	if (!Dentry || Dentry->flags != 1) return GF_BAD_PARAM;

	//Open our data map. We are adding stuff, so use EDIT
	e = gf_isom_datamap_open(trak->Media, dataRefIndex, 1);
	if (e) return e;

	data_offset = gf_isom_datamap_get_offset(trak->Media->information->dataHandler);
	if (offset_times) sample->DTS += 1;

	/*REWRITE ANY OD STUFF*/
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_OD) {
		GF_ISOSample *od_sample = NULL;
		e = Media_ParseODFrame(trak->Media, sample, &od_sample);
		if (e) return e;

		e = trak_add_sample(movie, trak, od_sample, descIndex, data_offset, sampleNum);
		if (od_sample)
			gf_isom_sample_del(&od_sample);
	} else {
		e = trak_add_sample(movie, trak, sample, descIndex, data_offset, sampleNum);
	}
	if (e) return e;
	if (offset_times) sample->DTS -= 1;

	//OK, update duration
	e = Media_SetDuration(trak);
	if (e) return e;
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	return SetTrackDuration(trak);
}