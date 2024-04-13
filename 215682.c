GF_Err gf_isom_remove_chapter(GF_ISOFile *movie, u32 trackNumber, u32 index)
{
	GF_Err e;
	GF_ChapterListBox *ptr;
	GF_ChapterEntry *ce;
	GF_UserDataBox *udta;
	GF_UserDataMap *map;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	e = gf_isom_insert_moov(movie);
	if (e) return e;

	if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		if (!trak->udta) return GF_OK;
		udta = trak->udta;
	} else {
		if (!movie->moov->udta) return GF_OK;
		udta = movie->moov->udta;
	}

	map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_CHPL, NULL);
	if (!map) return GF_OK;
	ptr = (GF_ChapterListBox*)gf_list_get(map->boxes, 0);
	if (!ptr) return GF_OK;

	if (index) {
		ce = (GF_ChapterEntry *)gf_list_get(ptr->list, index-1);
		if (!ce) return GF_BAD_PARAM;
		if (ce->name) gf_free(ce->name);
		gf_free(ce);
		gf_list_rem(ptr->list, index-1);
	} else {
		while (gf_list_count(ptr->list)) {
			ce = (GF_ChapterEntry *)gf_list_get(ptr->list, 0);
			if (ce->name) gf_free(ce->name);
			gf_free(ce);
			gf_list_rem(ptr->list, 0);
		}
	}
	if (!gf_list_count(ptr->list)) {
		gf_list_del_item(udta->recordList, map);
		gf_isom_box_array_del(map->boxes);
		gf_free(map);
	}
	return GF_OK;
}