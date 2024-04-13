GF_Err gf_isom_set_sync_table(GF_ISOFile *file, u32 track)
{
	GF_Err e;
	GF_TrackBox *trak;

	e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;

	if (!trak->Media->information->sampleTable->SyncSample) {
		trak->Media->information->sampleTable->SyncSample = (GF_SyncSampleBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_STSS);

		if (!trak->Media->information->sampleTable->SyncSample)
			return GF_OUT_OF_MEM;
	}
	return GF_OK;
}