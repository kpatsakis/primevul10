GF_Err gf_isom_set_edit(GF_ISOFile *movie, u32 trackNumber, u64 EditTime, u64 EditDuration, u64 MediaTime, GF_ISOEditType EditMode)
{
	return gf_isom_set_edit_internal(movie, trackNumber, EditTime, EditDuration, MediaTime, 0, EditMode);
}