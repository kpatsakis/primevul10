GF_Err gf_isom_set_copyright(GF_ISOFile *movie, const char *threeCharCode, char *notice)
{
	GF_Err e;
	GF_CopyrightBox *ptr;
	GF_UserDataMap *map;
	u32 count, i;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (!notice || !threeCharCode) return GF_BAD_PARAM;

	e = gf_isom_insert_moov(movie);
	if (e) return e;

	if (!movie->moov->udta) {
		e = moov_on_child_box((GF_Box*)movie->moov, gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
		if (e) return e;
	}
	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_CPRT, NULL);

	if (map) {
		//try to find one in our language...
		count = gf_list_count(map->boxes);
		for (i=0; i<count; i++) {
			ptr = (GF_CopyrightBox*)gf_list_get(map->boxes, i);
			if (!strcmp(threeCharCode, (const char *) ptr->packedLanguageCode)) {
				gf_free(ptr->notice);
				ptr->notice = (char*)gf_malloc(sizeof(char) * (strlen(notice) + 1));
				if (!ptr->notice) return GF_OUT_OF_MEM;
				strcpy(ptr->notice, notice);
				return GF_OK;
			}
		}
	}
	//nope, create one
	ptr = (GF_CopyrightBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_CPRT);
	if (!ptr) return GF_OUT_OF_MEM;

	memcpy(ptr->packedLanguageCode, threeCharCode, 4);
	ptr->notice = (char*)gf_malloc(sizeof(char) * (strlen(notice)+1));
	if (!ptr->notice) return GF_OUT_OF_MEM;
	strcpy(ptr->notice, notice);
	return udta_on_child_box((GF_Box *)movie->moov->udta, (GF_Box *) ptr, GF_FALSE);
}