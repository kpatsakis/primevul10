GF_Err gf_isom_set_watermark(GF_ISOFile *movie, bin128 UUID, u8* data, u32 length)
{
	GF_Err e;
	GF_UnknownUUIDBox *ptr;
	GF_UserDataMap *map;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	e = gf_isom_insert_moov(movie);
	if (e) return e;
	if (!movie->moov->udta) {
		e = moov_on_child_box((GF_Box*)movie->moov, gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA));
		if (e) return e;
	}

	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_UUID, (bin128 *) & UUID);
	if (map) {
		ptr = (GF_UnknownUUIDBox *)gf_list_get(map->boxes, 0);
		if (ptr) {
			gf_free(ptr->data);
			ptr->data = (char*)gf_malloc(length);
			if (!ptr->data) return GF_OUT_OF_MEM;
			memcpy(ptr->data, data, length);
			ptr->dataSize = length;
			return GF_OK;
		}
	}
	//nope, create one
	ptr = (GF_UnknownUUIDBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_UUID);
	if (!ptr) return GF_OUT_OF_MEM;

	memcpy(ptr->uuid, UUID, 16);
	ptr->data = (char*)gf_malloc(length);
	if (!ptr->data) return GF_OUT_OF_MEM;
	memcpy(ptr->data, data, length);
	ptr->dataSize = length;
	return udta_on_child_box((GF_Box *)movie->moov->udta, (GF_Box *) ptr);
}