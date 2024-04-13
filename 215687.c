GF_Err gf_isom_append_sample_data(GF_ISOFile *movie, u32 trackNumber, u8 *data, u32 data_size)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	u32 dataRefIndex;
	u32 descIndex;
	GF_DataEntryURLBox *Dentry;

	if (!data_size) return GF_OK;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_OD) return GF_BAD_PARAM;

	//OK, add the sample
	descIndex = trak->Media->information->sampleTable->currentEntryIndex;

	e = Media_GetSampleDesc(trak->Media, descIndex, &entry, &dataRefIndex);
	if (e) return e;
	if (!entry || !dataRefIndex) return GF_BAD_PARAM;

	//get this dataRef and return false if not self contained
	Dentry = (GF_DataEntryURLBox*)gf_list_get(trak->Media->information->dataInformation->dref->child_boxes, dataRefIndex - 1);
	if (!Dentry || Dentry->flags != 1) return GF_BAD_PARAM;

	//Open our data map. We are adding stuff, so use EDIT
	e = gf_isom_datamap_open(trak->Media, dataRefIndex, 1);
	if (e) return e;

	//add the media data
	if (trak->chunk_cache) {
		gf_bs_write_data(trak->chunk_cache, data, data_size);
		trak->chunk_cache_size += data_size;
	} else {
		e = gf_isom_datamap_add_data(trak->Media->information->dataHandler, data, data_size);
		if (e) return e;
	}
	//update data size
	return stbl_SampleSizeAppend(trak->Media->information->sampleTable->SampleSize, data_size);
}