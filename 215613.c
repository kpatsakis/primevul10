GF_Err gf_isom_add_chapter(GF_ISOFile *movie, u32 trackNumber, u64 timestamp, char *name)
{
	GF_Err e;
	GF_ChapterListBox *ptr;
	u32 i, count;
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
		if (!trak->udta) {
			e = trak_on_child_box((GF_Box*)trak, gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
			if (e) return e;
		}
		udta = trak->udta;
	} else {
		if (!movie->moov->udta) {
			e = moov_on_child_box((GF_Box*)movie->moov, gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
			if (e) return e;
		}
		udta = movie->moov->udta;
	}

	ptr = NULL;
	map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_CHPL, NULL);
	if (!map) {
		ptr = (GF_ChapterListBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_CHPL);
		e = udta_on_child_box((GF_Box *)udta, (GF_Box *) ptr, GF_FALSE);
		if (e) return e;
		map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_CHPL, NULL);
	} else {
		ptr = (GF_ChapterListBox*)gf_list_get(map->boxes, 0);
	}
	if (!map) return GF_OUT_OF_MEM;

	/*this may happen if original MP4 is not properly formatted*/
	if (!ptr) {
		ptr = (GF_ChapterListBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_CHPL);
		if (!ptr) return GF_OUT_OF_MEM;
		gf_list_add(map->boxes, ptr);
	}

	GF_SAFEALLOC(ce, GF_ChapterEntry);
	if (!ce) return GF_OUT_OF_MEM;

	ce->start_time = timestamp * 10000L;
	ce->name = name ? gf_strdup(name) : NULL;

	/*insert in order*/
	count = gf_list_count(ptr->list);
	for (i=0; i<count; i++) {
		GF_ChapterEntry *ace = (GF_ChapterEntry *)gf_list_get(ptr->list, i);
		if (ace->start_time == ce->start_time) {
			if (ace->name) gf_free(ace->name);
			ace->name = ce->name;
			gf_free(ce);
			return GF_OK;
		}
		if (ace->start_time >= ce->start_time)
			return gf_list_insert(ptr->list, ce, i);
	}
	return gf_list_add(ptr->list, ce);
}