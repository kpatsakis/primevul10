GF_Err gf_isom_box_add_default(GF_Box *a, GF_Box *subbox)
{
	if (!a->other_boxes) {
		a->other_boxes = gf_list_new();
		if (!a->other_boxes) return GF_OUT_OF_MEM;
	}
	return gf_list_add(a->other_boxes, subbox);
}