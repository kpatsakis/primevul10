GF_Err gf_isom_add_sample(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, const GF_ISOSample *sample)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	u32 dataRefIndex;
	u64 data_offset;
	u32 descIndex;
	GF_DataEntryURLBox *Dentry;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	e = FlushCaptureMode(movie);
	if (e) return e;

	e = unpack_track(trak);
	if (e) return e;

	//OK, add the sample
	//1- Get the streamDescriptionIndex and dataRefIndex
	//not specified, get the latest used...
	descIndex = StreamDescriptionIndex;
	if (!StreamDescriptionIndex) {
		descIndex = trak->Media->information->sampleTable->currentEntryIndex;
	}
	e = Media_GetSampleDesc(trak->Media, descIndex, &entry, &dataRefIndex);
	if (e) return e;
	if (!entry || !dataRefIndex) return GF_BAD_PARAM;
	//set the current to this one
	trak->Media->information->sampleTable->currentEntryIndex = descIndex;


	//get this dataRef and return false if not self contained
	Dentry = (GF_DataEntryURLBox*)gf_list_get(trak->Media->information->dataInformation->dref->child_boxes, dataRefIndex - 1);
	if (!Dentry || Dentry->flags != 1) return GF_BAD_PARAM;

	//Open our data map. We are adding stuff, so use EDIT
	e = gf_isom_datamap_open(trak->Media, dataRefIndex, 1);
	if (e) return e;

	//Get the offset...
	data_offset = gf_isom_datamap_get_offset(trak->Media->information->dataHandler);

	/*rewrite OD frame*/
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_OD) {
		GF_ISOSample *od_sample = NULL;

		e = Media_ParseODFrame(trak->Media, sample, &od_sample);
		if (e) return e;

		e = trak_add_sample(movie, trak, od_sample, descIndex, data_offset, 0);

		if (od_sample)
			gf_isom_sample_del(&od_sample);
	} else {
		e = trak_add_sample(movie, trak, sample, descIndex, data_offset, 0);
	}
	if (e) return e;


	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	return SetTrackDuration(trak);
}