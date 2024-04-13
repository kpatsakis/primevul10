GF_Err gf_isom_set_edit_with_rate(GF_ISOFile *movie, u32 trackNumber, u64 EditTime, u64 EditDuration, u64 MediaTime, u32 media_rate)
{
	return gf_isom_set_edit_internal(movie, trackNumber, EditTime, EditDuration, MediaTime, media_rate, GF_ISOM_EDIT_NORMAL+1);

}