void reset_tsel_box(GF_TrackBox *trak)
{
	GF_UserDataMap *map;
	trak->Header->alternate_group = 0;
	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_TSEL, NULL);
	if (map) {
		gf_list_del_item(trak->udta->recordList, map);
		gf_isom_box_array_del(map->boxes);
		gf_free(map);
	}

}