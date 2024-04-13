GF_Err gf_isom_get_raw_user_data(GF_ISOFile *file, u8 **output, u32 *output_size)
{
	GF_BitStream *bs;
	GF_Err e;
	GF_Box *b;
	u32 i;

	*output = NULL;
	*output_size = 0;
	if (!file || !file->moov || (!file->moov->udta && !file->moov->child_boxes)) return GF_OK;
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	if (file->moov->udta) {
		e = gf_isom_box_size( (GF_Box *) file->moov->udta);
		if (e) goto exit;
		e = gf_isom_box_write((GF_Box *) file->moov->udta, bs);
		if (e) goto exit;
	}
	e = GF_OK;
	i=0;
	while ((b = gf_list_enum(file->moov->child_boxes, &i))) {
		switch (b->type) {
		case GF_ISOM_BOX_TYPE_TRAK:
		case GF_ISOM_BOX_TYPE_MVHD:
		case GF_ISOM_BOX_TYPE_MVEX:
		case GF_ISOM_BOX_TYPE_IODS:
		case GF_ISOM_BOX_TYPE_META:
			continue;
		}
		e = gf_isom_box_size( (GF_Box *) b);
		if (e) goto exit;
		e = gf_isom_box_write((GF_Box *) b, bs);
		if (e) goto exit;
	}

	gf_bs_get_content(bs, output, output_size);

exit:
	gf_bs_del(bs);
	return e;
}