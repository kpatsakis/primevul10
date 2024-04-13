GF_Err gf_isom_update_duration(GF_ISOFile *movie)
{
	u32 i;
	u64 maxDur;
	GF_TrackBox *trak;

	if (!movie || !movie->moov) return GF_BAD_PARAM;

	//if file was open in Write or Edit mode, recompute the duration
	//the duration of a movie is the MaxDuration of all the tracks...

	maxDur = 0;
	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(movie->moov->trackList, &i))) {
		if( (movie->LastError = SetTrackDuration(trak))	) return movie->LastError;
		if (trak->Header && (trak->Header->duration > maxDur))
			maxDur = trak->Header->duration;
	}
	movie->moov->mvhd->duration = maxDur;

	return GF_OK;
}