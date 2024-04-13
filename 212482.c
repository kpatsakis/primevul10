void gf_isom_box_array_del(GF_List *other_boxes)
{
	u32 count, i;
	GF_Box *a;
	if (!other_boxes) return;
	count = gf_list_count(other_boxes);
	for (i = 0; i < count; i++) {
		a = (GF_Box *)gf_list_get(other_boxes, i);
		if (a) gf_isom_box_del(a);
	}
	gf_list_del(other_boxes);
}