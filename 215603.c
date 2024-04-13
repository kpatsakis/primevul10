GF_Err gf_isom_set_track_matrix(GF_ISOFile *the_file, u32 trackNumber, s32 matrix[9])
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Header) return GF_BAD_PARAM;
	memcpy(trak->Header->matrix, matrix, sizeof(trak->Header->matrix));
	return GF_OK;
}