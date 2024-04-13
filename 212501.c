GF_Err gf_isom_box_array_read(GF_Box *parent, GF_BitStream *bs, GF_Err (*add_box)(GF_Box *par, GF_Box *b))
{
	return gf_isom_box_array_read_ex(parent, bs, add_box, parent->type);
}