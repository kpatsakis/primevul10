GF_Err gf_isom_get_stsd_template(GF_ISOFile *file, u32 track, u32 stsd_idx, u8 **output, u32 *output_size)
{
	GF_TrackBox *trak;
	GF_BitStream *bs;
	GF_Box *ent;

	*output = NULL;
	*output_size = 0;
	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(file, track);
	if (!trak || !stsd_idx || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->SampleDescription) return GF_BAD_PARAM;

	ent = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, stsd_idx-1);
	if (!ent) return GF_BAD_PARAM;

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	gf_isom_box_size( (GF_Box *) ent);
	gf_isom_box_write((GF_Box *) ent, bs);
	gf_bs_get_content(bs, output, output_size);
	gf_bs_del(bs);
	return GF_OK;

}