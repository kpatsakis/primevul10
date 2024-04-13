GF_Err gf_isom_remove_user_data(GF_ISOFile *movie, u32 trackNumber, u32 UserDataType, bin128 UUID)
{
	GF_UserDataMap *map;
	u32 i;
	GF_Err e;
	bin128 t;
	GF_TrackBox *trak;
	GF_UserDataBox *udta;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (UserDataType == GF_ISOM_BOX_TYPE_UUID) UserDataType = 0;
	memset(t, 1, 16);

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_EOS;
		udta = trak->udta;
	} else {
		udta = movie->moov->udta;
	}
	//do not return any error if no udta
	if (!udta) return GF_EOS;

	i=0;
	while ((map = (GF_UserDataMap*)gf_list_enum(udta->recordList, &i))) {
		if ((map->boxType == GF_ISOM_BOX_TYPE_UUID)  && !memcmp(map->uuid, UUID, 16)) goto found;
		else if (map->boxType == UserDataType) goto found;
	}
	//not found
	return GF_OK;

found:

	gf_list_rem(udta->recordList, i-1);
	gf_isom_box_array_del(map->boxes);
	gf_free(map);

	//but we keep the UDTA no matter what
	return GF_OK;
}