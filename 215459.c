
GF_Err trgr_box_read(GF_Box *s, GF_BitStream *bs)
{
	return gf_isom_box_array_read_ex(s, bs, trgr_on_child_box, s->type);