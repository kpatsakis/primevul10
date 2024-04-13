GF_Err gf_isom_update_sample_description_from_template(GF_ISOFile *file, u32 track, u32 sampleDescriptionIndex, u8 *data, u32 size)
{
	GF_BitStream *bs;
	GF_TrackBox *trak;
	GF_Box *ent, *tpl_ent;
	GF_Err e;
	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(file, track);
	if (!trak || !sampleDescriptionIndex) return GF_BAD_PARAM;

	if (!trak->Media || !trak->Media->handler || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->SampleDescription)
		return GF_ISOM_INVALID_FILE;

	ent = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescriptionIndex-1);
	if (!ent) return GF_BAD_PARAM;

	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
//	e = gf_isom_box_parse(&tpl_ent, bs);
	e = gf_isom_box_parse_ex (&tpl_ent, bs, GF_ISOM_BOX_TYPE_STSD, GF_FALSE);
	gf_bs_del(bs);
	if (e) return e;

	while (gf_list_count(tpl_ent->child_boxes)) {
		u32 j=0;
		Bool found = GF_FALSE;
		GF_Box *abox = gf_list_pop_front(tpl_ent->child_boxes);

		switch (abox->type) {
		case GF_ISOM_BOX_TYPE_SINF:
		case GF_ISOM_BOX_TYPE_RINF:
		case GF_ISOM_BOX_TYPE_BTRT:
			found = GF_TRUE;
			break;
		}

		if (found) {
			gf_isom_box_del(abox);
			continue;
		}
		
		if (!ent->child_boxes) ent->child_boxes = gf_list_new();
		for (j=0; j<gf_list_count(ent->child_boxes); j++) {
			GF_Box *b = gf_list_get(ent->child_boxes, j);
			if (b->type == abox->type) {
				found = GF_TRUE;
				break;
			}
		}
		if (!found) {
			gf_list_add(ent->child_boxes, abox);
		} else {
			gf_isom_box_del(abox);
		}
	}
	gf_isom_box_del(tpl_ent);

	//patch for old export
	GF_Box *abox = gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_SINF);
	if (abox) {
		gf_list_del_item(ent->child_boxes, abox);
		gf_list_add(ent->child_boxes, abox);
	}
	return GF_OK;
}