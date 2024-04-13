Bool gf_isom_enable_raw_pack(GF_ISOFile *the_file, u32 trackNumber, u32 pack_num_samples)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	trak->pack_num_samples = 0;
	//we only activate sample packing for raw audio
	if (!trak->Media || !trak->Media->handler) return GF_FALSE;
	if (trak->Media->handler->handlerType != GF_ISOM_MEDIA_AUDIO) return GF_FALSE;
	//and sample duration of 1
	if (!trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->TimeToSample) return GF_FALSE;
	if (trak->Media->information->sampleTable->TimeToSample->nb_entries != 1) return GF_FALSE;
	if (!trak->Media->information->sampleTable->TimeToSample->entries) return GF_FALSE;
	if (trak->Media->information->sampleTable->TimeToSample->entries[0].sampleDelta != 1) return GF_FALSE;
	//and sample with constant size
	if (!trak->Media->information->sampleTable->SampleSize || !trak->Media->information->sampleTable->SampleSize->sampleSize) return GF_FALSE;
	trak->pack_num_samples = pack_num_samples;
	return pack_num_samples ? GF_TRUE : GF_FALSE;
}