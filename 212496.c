void gf_isom_box_add_for_dump_mode(GF_Box *parent, GF_Box *a)
{
	if (use_dump_mode && a && (!parent->other_boxes || (gf_list_find(parent->other_boxes, a)<0) ) )
		gf_isom_box_add_default(parent, a);
}