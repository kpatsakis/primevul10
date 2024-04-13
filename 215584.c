GF_Err gf_isom_get_trex_template(GF_ISOFile *file, u32 track, u8 **output, u32 *output_size)
{
	GF_TrackBox *trak;
	GF_BitStream *bs;
	u32 i;
	GF_TrackExtendsBox *trex = NULL;
	GF_TrackExtensionPropertiesBox *trexprop = NULL;

	*output = NULL;
	*output_size = 0;
	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(file, track);
	if (!trak || !trak->Media) return GF_BAD_PARAM;
	if (!file->moov->mvex) return GF_NOT_FOUND;
	for (i=0; i<gf_list_count(file->moov->mvex->TrackExList); i++) {
		trex = gf_list_get(file->moov->mvex->TrackExList, i);
		if (trex->trackID == trak->Header->trackID) break;
		trex = NULL;
	}
	if (!trex) return GF_NOT_FOUND;

	for (i=0; i<gf_list_count(file->moov->mvex->TrackExPropList); i++) {
		trexprop = gf_list_get(file->moov->mvex->TrackExPropList, i);
		if (trexprop->trackID== trak->Header->trackID) break;
		trexprop = NULL;
	}
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_isom_box_size( (GF_Box *) trex);
	gf_isom_box_write((GF_Box *) trex, bs);

	if (trexprop) {
		gf_isom_box_size( (GF_Box *) trexprop);
		gf_isom_box_write((GF_Box *) trexprop, bs);
	}
	gf_bs_get_content(bs, output, output_size);
	gf_bs_del(bs);

	return GF_OK;

}