GF_Err gf_isom_set_timescale(GF_ISOFile *movie, u32 timeScale)
{
	GF_TrackBox *trak;
	u32 i;
	GF_Err e;
	if (!timeScale) return GF_BAD_PARAM;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	e = gf_isom_insert_moov(movie);
	if (e) return e;

	if (movie->moov->mvhd->timeScale == timeScale) return GF_OK;

	/*rewrite all durations and edit lists*/
	movie->moov->mvhd->duration *= timeScale;
	movie->moov->mvhd->duration /= movie->moov->mvhd->timeScale;
	if (movie->moov->mvex && movie->moov->mvex->mehd) {
		movie->moov->mvex->mehd->fragment_duration *= timeScale;
		movie->moov->mvex->mehd->fragment_duration /= movie->moov->mvhd->timeScale;
	}

	i=0;
	while ((trak = (GF_TrackBox*)gf_list_enum(movie->moov->trackList, &i))) {
		trak->Header->duration *= timeScale;
		trak->Header->duration /= movie->moov->mvhd->timeScale;

		if (trak->editBox && trak->editBox->editList) {
			u32 j, count = gf_list_count(trak->editBox->editList->entryList);
			for (j=0; j<count; j++) {
				GF_EdtsEntry *ent = (GF_EdtsEntry *)gf_list_get(trak->editBox->editList->entryList, j);
				ent->segmentDuration *= timeScale;
				ent->segmentDuration /= movie->moov->mvhd->timeScale;
			}
		}
	}
	if (movie->moov->mvex && movie->moov->mvex->mehd) {
		movie->moov->mvex->mehd->fragment_duration *= timeScale;
		movie->moov->mvex->mehd->fragment_duration /= movie->moov->mvhd->timeScale;
	}
	movie->moov->mvhd->timeScale = timeScale;
	movie->interleavingTime = timeScale;
	return GF_OK;
}