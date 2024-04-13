GF_Err gf_isom_remove_copyright(GF_ISOFile *movie, u32 index)
{
	GF_Err e;
	GF_CopyrightBox *ptr;
	GF_UserDataMap *map;
	u32 count;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	e = gf_isom_insert_moov(movie);
	if (e) return e;

	if (!index) return GF_BAD_PARAM;
	if (!movie->moov->udta) return GF_OK;

	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_CPRT, NULL);
	if (!map) return GF_OK;

	count = gf_list_count(map->boxes);
	if (index>count) return GF_BAD_PARAM;

	ptr = (GF_CopyrightBox*)gf_list_get(map->boxes, index-1);
	if (ptr) {
		gf_list_rem(map->boxes, index-1);
		if (ptr->notice) gf_free(ptr->notice);
		gf_free(ptr);
	}
	/*last copyright, remove*/
	if (!gf_list_count(map->boxes)) {
		gf_list_del_item(movie->moov->udta->recordList, map);
		gf_list_del(map->boxes);
		gf_free(map);
	}
	return GF_OK;
}