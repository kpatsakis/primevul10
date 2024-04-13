GF_Err gf_isom_full_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_FullBox *ptr = (GF_FullBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->version);
	gf_bs_write_u24(bs, ptr->flags);
	return GF_OK;
}