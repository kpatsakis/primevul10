static GF_Err unpack_track(GF_TrackBox *trak)
{
	GF_Err e = GF_OK;
	if (!trak->is_unpacked) {
		e = stbl_UnpackOffsets(trak->Media->information->sampleTable);
		if (e) return e;
		e = stbl_unpackCTS(trak->Media->information->sampleTable);
		trak->is_unpacked = GF_TRUE;
	}
	return e;
}