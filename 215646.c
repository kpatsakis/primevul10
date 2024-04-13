GF_Err gf_isom_load_extra_boxes(GF_ISOFile *movie, u8 *moov_boxes, u32 moov_boxes_size, Bool udta_only)
{
	GF_BitStream *bs;

	GF_Err e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	e = gf_isom_insert_moov(movie);
	if (e) return e;

	bs = gf_bs_new(moov_boxes, moov_boxes_size, GF_BITSTREAM_READ);

	//we may have terminators in some QT files (4 bytes set to 0 ...)
	while (gf_bs_available(bs) >= 8) {
		GF_Box *a_box;
		e = gf_isom_box_parse_ex((GF_Box**)&a_box, bs, GF_ISOM_BOX_TYPE_MOOV, GF_FALSE);
		if (e || !a_box) goto exit;

		if (a_box->type == GF_ISOM_BOX_TYPE_UDTA) {
			if (movie->moov->udta) gf_isom_box_del_parent(&movie->moov->child_boxes, (GF_Box*)movie->moov->udta);
			movie->moov->udta = (GF_UserDataBox*) a_box;

			if (!movie->moov->child_boxes) movie->moov->child_boxes = gf_list_new();
			gf_list_add(movie->moov->child_boxes, a_box);

		} else if (!udta_only && (a_box->type!=GF_ISOM_BOX_TYPE_PSSH) ) {
			if (!movie->moov->child_boxes) movie->moov->child_boxes = gf_list_new();
			gf_list_add(movie->moov->child_boxes, a_box);
		} else {
			gf_isom_box_del(a_box);
		}
	}
exit:
	gf_bs_del(bs);
	return e;
}