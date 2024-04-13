GF_Err gf_isom_get_track_template(GF_ISOFile *file, u32 track, u8 **output, u32 *output_size)
{
	GF_TrackBox *trak;
	GF_BitStream *bs;
	GF_DataReferenceBox *dref;
	GF_SampleTableBox *stbl, *stbl_temp;
	GF_SampleEncryptionBox *senc;
	u32 i, count;

	*output = NULL;
	*output_size = 0;
	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(file, track);
	if (!trak || !trak->Media) return GF_BAD_PARAM;

	//don't serialize dref
	dref = NULL;
	if (trak->Media->information->dataInformation) {
		dref = trak->Media->information->dataInformation->dref;
		trak->Media->information->dataInformation->dref = NULL;
		gf_list_del_item(trak->Media->information->dataInformation->child_boxes, dref);
	}

	//don't serialize stbl but create a temp one
	stbl_temp = (GF_SampleTableBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_STBL);
	if (!stbl_temp->child_boxes) stbl_temp->child_boxes = gf_list_new();
	stbl = trak->Media->information->sampleTable;
	gf_list_del_item(trak->Media->information->child_boxes, stbl);

	trak->Media->information->sampleTable = stbl_temp;
	gf_list_add(trak->Media->information->child_boxes, stbl_temp);

	/*do not clone sampleDescription table but create an empty one*/
	stbl_temp->SampleDescription = (GF_SampleDescriptionBox *) gf_isom_box_new_parent(&stbl_temp->child_boxes, GF_ISOM_BOX_TYPE_STSD);

	/*clone sampleGroups description tables if any*/
	stbl_temp->sampleGroupsDescription = stbl->sampleGroupsDescription;
	count = gf_list_count(stbl->sampleGroupsDescription);
	for (i=0;i<count; i++) {
		GF_Box *b = gf_list_get(stbl->sampleGroupsDescription, i);
		gf_list_add(stbl_temp->child_boxes, b);
	}
	/*clone CompositionToDecode table, we may remove it later*/
	stbl_temp->CompositionToDecode = stbl->CompositionToDecode;
	gf_list_add(stbl_temp->child_boxes, stbl->CompositionToDecode);


	//don't serialize senc
	senc = trak->sample_encryption;
	if (senc) {
		assert(trak->child_boxes);
		gf_list_del_item(trak->child_boxes, senc);
		trak->sample_encryption = NULL;
	}

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	gf_isom_box_size( (GF_Box *) trak);
	gf_isom_box_write((GF_Box *) trak, bs);
	gf_bs_get_content(bs, output, output_size);
	gf_bs_del(bs);

	//restore our pointers
	if (dref) {
		trak->Media->information->dataInformation->dref = dref;
		gf_list_add(trak->Media->information->dataInformation->child_boxes, dref);
	}
	trak->Media->information->sampleTable = stbl;
	gf_list_add(trak->Media->information->child_boxes, stbl);
	gf_list_del_item(trak->Media->information->child_boxes, stbl_temp);
	if (senc) {
		trak->sample_encryption = senc;
		gf_list_add(trak->child_boxes, senc);
	}

	stbl_temp->sampleGroupsDescription = NULL;
	count = gf_list_count(stbl->sampleGroupsDescription);
	for (i=0;i<count; i++) {
		GF_Box *b = gf_list_get(stbl->sampleGroupsDescription, i);
		gf_list_del_item(stbl_temp->child_boxes, b);
	}

	stbl_temp->CompositionToDecode = NULL;
	gf_list_del_item(stbl_temp->child_boxes, stbl->CompositionToDecode);
	gf_isom_box_del((GF_Box *)stbl_temp);
	return GF_OK;

}