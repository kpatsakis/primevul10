GF_Err gf_isom_clone_box(GF_Box *src, GF_Box **dst)
{
	GF_Err e;
	u8 *data;
	u32 data_size;
	GF_BitStream *bs;

	if (*dst) {
		gf_isom_box_del(*dst);
		*dst=NULL;
	}
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	if (!bs) return GF_OUT_OF_MEM;
	e = gf_isom_box_size( (GF_Box *) src);
	if (!e) e = gf_isom_box_write((GF_Box *) src, bs);
	gf_bs_get_content(bs, &data, &data_size);
	gf_bs_del(bs);
	if (e) {
		if (data) gf_free(data);
		return e;
	}
	bs = gf_bs_new(data, data_size, GF_BITSTREAM_READ);
	if (!bs) {
		if (data) gf_free(data);
		return GF_OUT_OF_MEM;
	}
	e = gf_isom_box_parse(dst, bs);
	gf_bs_del(bs);
	gf_free(data);
	return e;
}