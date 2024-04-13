GF_Err gf_isom_add_user_data(GF_ISOFile *movie, u32 trackNumber, u32 UserDataType, bin128 UUID, u8 *data, u32 DataLength)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_UserDataBox *udta;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (UserDataType == GF_ISOM_BOX_TYPE_UUID) UserDataType = 0;

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		if (!trak->udta) trak_on_child_box((GF_Box*)trak, gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
		udta = trak->udta;
	} else {
		if (!movie->moov->udta) moov_on_child_box((GF_Box*)movie->moov, gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
		udta = movie->moov->udta;
	}
	if (!udta) return GF_OUT_OF_MEM;

	//create a default box
	if (UserDataType) {
		GF_UnknownBox *a = (GF_UnknownBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_UNKNOWN);
		if (!a) return GF_OUT_OF_MEM;
		a->original_4cc = UserDataType;
		if (DataLength) {
			a->data = (char*)gf_malloc(sizeof(char)*DataLength);
			if (!a->data) return GF_OUT_OF_MEM;
			memcpy(a->data, data, DataLength);
			a->dataSize = DataLength;
		}
		return udta_on_child_box((GF_Box *)udta, (GF_Box *) a, GF_FALSE);
	} else {
		GF_UnknownUUIDBox *a = (GF_UnknownUUIDBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_UUID);
		if (!a) return GF_OUT_OF_MEM;
		memcpy(a->uuid, UUID, 16);
		if (DataLength) {
			a->data = (char*)gf_malloc(sizeof(char)*DataLength);
			if (!a->data) return GF_OUT_OF_MEM;
			memcpy(a->data, data, DataLength);
			a->dataSize = DataLength;
		}
		return udta_on_child_box((GF_Box *)udta, (GF_Box *) a, GF_FALSE);
	}
	return GF_OK;
}