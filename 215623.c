GF_Err gf_isom_set_sample_padding_bits(GF_ISOFile *movie, u32 trackNumber, u32 sampleNumber, u8 NbBits)
{
	GF_TrackBox *trak;
	GF_Err e;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || NbBits > 7) return GF_BAD_PARAM;

	//set Padding info
	return stbl_SetPaddingBits(trak->Media->information->sampleTable, sampleNumber, NbBits);
}