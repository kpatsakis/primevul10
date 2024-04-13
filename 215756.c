GF_Err gf_isom_clone_sample_description(GF_ISOFile *the_file, u32 trackNumber, GF_ISOFile *orig_file, u32 orig_track, u32 orig_desc_index, const char *URLname, const char *URNname, u32 *outDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_BitStream *bs;
	u8 *data;
	u32 data_size;
	GF_Box *entry;
	GF_Err e;
	u32 dataRefIndex;
    u32 mtype;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	/*get orig sample desc and clone it*/
	trak = gf_isom_get_track_from_file(orig_file, orig_track);
	if (!trak || !trak->Media) return GF_BAD_PARAM;

	entry = (GF_Box*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, orig_desc_index-1);
	if (!entry) return GF_BAD_PARAM;

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	gf_isom_box_size(entry);
	gf_isom_box_write(entry, bs);
	gf_bs_get_content(bs, &data, &data_size);
	gf_bs_del(bs);
	bs = gf_bs_new(data, data_size, GF_BITSTREAM_READ);
	e = gf_isom_box_parse(&entry, bs);
	gf_bs_del(bs);
	gf_free(data);
	if (e) return e;

	/*get new track and insert clone*/
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media) goto exit;

	/*get or create the data ref*/
	e = Media_FindDataRef(trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
	if (e) goto exit;
	if (!dataRefIndex) {
		e = Media_CreateDataRef(the_file, trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
		if (e) goto exit;
	}
	if (!the_file->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();
	/*overwrite dref*/
	((GF_SampleEntryBox *)entry)->dataReferenceIndex = dataRefIndex;
	e = gf_list_add(trak->Media->information->sampleTable->SampleDescription->child_boxes, entry);
	*outDescriptionIndex = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);

	/*also clone track w/h info*/
    mtype = gf_isom_get_media_type(the_file, trackNumber);
	if (gf_isom_is_video_handler_type(mtype) ) {
		gf_isom_set_visual_info(the_file, trackNumber, (*outDescriptionIndex), ((GF_VisualSampleEntryBox*)entry)->Width, ((GF_VisualSampleEntryBox*)entry)->Height);
	}
	return e;

exit:
	gf_isom_box_del(entry);
	return e;
}