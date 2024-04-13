}
GF_Err jp2h_box_read(GF_Box *s,GF_BitStream *bs)
{
	return gf_isom_box_array_read_ex(s, bs, jp2h_on_child_box, s->type);