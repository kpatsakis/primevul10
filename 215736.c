Bool gf_isom_box_equal(GF_Box *a, GF_Box *b)
{
	Bool ret;
	u8 *data1, *data2;
	u32 data1_size, data2_size;
	GF_BitStream *bs;

	if (a == b) return GF_TRUE;
	if (!a || !b) return GF_FALSE;

	data1 = data2 = NULL;

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_isom_box_size(a);
	gf_isom_box_write(a, bs);
	gf_bs_get_content(bs, &data1, &data1_size);
	gf_bs_del(bs);

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_isom_box_size(b);
	gf_isom_box_write(b, bs);
	gf_bs_get_content(bs, &data2, &data2_size);
	gf_bs_del(bs);

	ret = GF_FALSE;
	if (data1_size == data2_size) {
		ret = (memcmp(data1, data2, sizeof(char)*data1_size) == 0) ? GF_TRUE : GF_FALSE;
	}
	gf_free(data1);
	gf_free(data2);
	return ret;
}