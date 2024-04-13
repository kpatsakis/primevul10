GF_Err gf_isom_add_track_kind(GF_ISOFile *movie, u32 trackNumber, const char *schemeURI, const char *value)
{
	GF_Err e;
	GF_KindBox *ptr;
	GF_UserDataBox *udta;
	GF_UserDataMap *map;
	u32 i, count;

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
		return GF_BAD_PARAM;
	}

	map = udta_getEntry(udta, GF_ISOM_BOX_TYPE_KIND, NULL);
	if (map) {
		count = gf_list_count(map->boxes);
		for (i=0; i<count; i++) {
			GF_Box *b = (GF_Box *)gf_list_get(map->boxes, i);
			if (b->type == GF_ISOM_BOX_TYPE_KIND) {
				GF_KindBox *kb = (GF_KindBox *)b;
				if (!strcmp(kb->schemeURI, schemeURI) &&
				        ((value && kb->value && !strcmp(value, kb->value)) || (!value && !kb->value))) {
					// Already there
					return GF_OK;
				}
			}
		}
	}

	ptr = (GF_KindBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_KIND);
	if (e) return e;

	ptr->schemeURI = gf_strdup(schemeURI);
	if (value) ptr->value = gf_strdup(value);
	return udta_on_child_box((GF_Box *)udta, (GF_Box *) ptr, GF_FALSE);
}