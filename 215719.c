GF_Err gf_isom_add_uuid(GF_ISOFile *movie, u32 trackNumber, bin128 UUID, const u8 *data, u32 data_size)
{
	GF_List *list;
    u32 btype;
	GF_Box *box;
	GF_UnknownUUIDBox *uuidb;

	if (data_size && !data) return GF_BAD_PARAM;
	if (trackNumber==(u32) -1) {
		if (!movie) return GF_BAD_PARAM;
		list = movie->TopBoxes;
	} else if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		if (!trak->child_boxes) trak->child_boxes = gf_list_new();
		list = trak->child_boxes;
	} else {
		if (!movie) return GF_BAD_PARAM;
		if (!movie->moov->child_boxes) movie->moov->child_boxes = gf_list_new();
		list = movie->moov->child_boxes;
	}
    btype = gf_isom_solve_uuid_box((char *) UUID);
    if (!btype) btype = GF_ISOM_BOX_TYPE_UUID;
    box = gf_isom_box_new(btype);
    if (!box) return GF_OUT_OF_MEM;
	uuidb = (GF_UnknownUUIDBox*)box;
	uuidb->internal_4cc = gf_isom_solve_uuid_box((char *) UUID);
	memcpy(uuidb->uuid, UUID, sizeof(bin128));
	uuidb->dataSize = data_size;
	if (data_size) {
		uuidb->data = (char*)gf_malloc(sizeof(char)*data_size);
		if (!uuidb->data) return GF_OUT_OF_MEM;
		memcpy(uuidb->data, data, sizeof(char)*data_size);
	}
	gf_list_add(list, uuidb);
	return GF_OK;
}