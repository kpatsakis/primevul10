GF_Err gf_isom_remove_uuid(GF_ISOFile *movie, u32 trackNumber, bin128 UUID)
{
	u32 i, count;
	GF_List *list;

	if (trackNumber==(u32) -1) {
		if (!movie) return GF_BAD_PARAM;
		list = movie->TopBoxes;
	} else if (trackNumber) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		list = trak->child_boxes;
	} else {
		if (!movie) return GF_BAD_PARAM;
		list = movie->moov->child_boxes;
	}

	count = list ? gf_list_count(list) : 0;
	for (i=0; i<count; i++) {
		GF_UnknownUUIDBox *uuid = (GF_UnknownUUIDBox *)gf_list_get(list, i);
		if (uuid->type != GF_ISOM_BOX_TYPE_UUID) continue;
		if (memcmp(UUID, uuid->uuid, sizeof(bin128))) continue;
		gf_list_rem(list, i);
		i--;
		count--;
		gf_isom_box_del((GF_Box*)uuid);
	}
	return GF_OK;
}