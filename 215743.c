GF_Err gf_isom_force_ctts(GF_ISOFile *file, u32 track)
{
	GF_TrackBox *trak;
	GF_Err e = CanAccessMovie(file, GF_ISOM_OPEN_WRITE);
	if (e) return e;
 	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return GF_BAD_PARAM;
	if (trak->Media->information->sampleTable->CompositionOffset) return GF_OK;

	trak->Media->information->sampleTable->CompositionOffset = (GF_CompositionOffsetBox *) gf_isom_box_new_parent(&trak->Media->information->sampleTable->child_boxes, GF_ISOM_BOX_TYPE_CTTS);
	if (!trak->Media->information->sampleTable->CompositionOffset) return GF_OUT_OF_MEM;
	trak->Media->information->sampleTable->CompositionOffset->nb_entries = 1;
	trak->Media->information->sampleTable->CompositionOffset->entries = gf_malloc(sizeof(GF_DttsEntry));
	trak->Media->information->sampleTable->CompositionOffset->entries[0].decodingOffset = 0;
	trak->Media->information->sampleTable->CompositionOffset->entries[0].sampleCount = 	trak->Media->information->sampleTable->SampleSize->sampleCount;
	return GF_OK;
}