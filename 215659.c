GF_Err gf_isom_add_user_data_boxes(GF_ISOFile *movie, u32 trackNumber, u8 *data, u32 DataLength)
{
	GF_Err e;
	GF_TrackBox *trak;
	GF_UserDataBox *udta;
	GF_BitStream *bs;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		if (!trak->udta) trak_on_child_box((GF_Box*)trak, gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
		udta = trak->udta;
	} else {
		if (!movie->moov) return GF_BAD_PARAM;
		if (!movie->moov->udta) moov_on_child_box((GF_Box*)movie->moov, gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
		udta = movie->moov->udta;
	}
	if (!udta) return GF_OUT_OF_MEM;

	bs = gf_bs_new(data, DataLength, GF_BITSTREAM_READ);
	while (gf_bs_available(bs)) {
		GF_Box *a;
		e = gf_isom_box_parse(&a, bs);
		if (e) break;
		e = udta_on_child_box((GF_Box *)udta, a, GF_FALSE);
		if (e) break;
	}
	gf_bs_del(bs);
	return e;
}