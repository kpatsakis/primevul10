GF_Err gf_isom_remove_sample(GF_ISOFile *movie, u32 trackNumber, u32 sampleNumber)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_EDIT);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !sampleNumber || (sampleNumber > trak->Media->information->sampleTable->SampleSize->sampleCount) )
		return GF_BAD_PARAM;

	//block for hint tracks
	if (trak->Media->handler->handlerType == GF_ISOM_MEDIA_HINT) return GF_BAD_PARAM;

	e = unpack_track(trak);
	if (e) return e;
	//do NOT change the order DTS, CTS, size chunk

	//remove DTS
	e = stbl_RemoveDTS(trak->Media->information->sampleTable, sampleNumber, 1, trak->Media->mediaHeader->timeScale);
	if (e) return e;
	//remove CTS if any
	if (trak->Media->information->sampleTable->CompositionOffset) {
		e = stbl_RemoveCTS(trak->Media->information->sampleTable, sampleNumber, 1);
		if (e) return e;
	}
	//remove size
	e = stbl_RemoveSize(trak->Media->information->sampleTable, sampleNumber, 1);
	if (e) return e;
	//remove sampleToChunk and chunk
	e = stbl_RemoveChunk(trak->Media->information->sampleTable, sampleNumber, 1);
	if (e) return e;
	//remove sync
	if (trak->Media->information->sampleTable->SyncSample) {
		e = stbl_RemoveRAP(trak->Media->information->sampleTable, sampleNumber);
		if (e) return e;
	}
	//remove sample dep
	if (trak->Media->information->sampleTable->SampleDep) {
		e = stbl_RemoveRedundant(trak->Media->information->sampleTable, sampleNumber, 1);
		if (e) return e;
	}
	//remove shadow
	e = stbl_RemoveShadow(trak->Media->information->sampleTable, sampleNumber);
	if (e) return e;

	//remove padding
	e = stbl_RemovePaddingBits(trak->Media->information->sampleTable, sampleNumber);
	if (e) return e;

	e = stbl_RemoveSubSample(trak->Media->information->sampleTable, sampleNumber);
	if (e) return e;

	e = stbl_RemoveSampleGroup(trak->Media->information->sampleTable, sampleNumber);
	if (e) return e;

	gf_isom_disable_inplace_rewrite(movie);

	return SetTrackDuration(trak);
}