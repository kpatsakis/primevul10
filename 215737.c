GF_Err gf_isom_set_ipod_compatible(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	GF_Err e;
	GF_MPEGVisualSampleEntryBox *entry;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media) return GF_BAD_PARAM;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, 0);
	if (!entry) return GF_OK;
	switch (entry->type) {
	case GF_ISOM_BOX_TYPE_AVC1:
	case GF_ISOM_BOX_TYPE_AVC2:
	case GF_ISOM_BOX_TYPE_AVC3:
	case GF_ISOM_BOX_TYPE_AVC4:
	case GF_ISOM_BOX_TYPE_SVC1:
	case GF_ISOM_BOX_TYPE_MVC1:
	case GF_ISOM_BOX_TYPE_HVC1:
	case GF_ISOM_BOX_TYPE_HEV1:
	case GF_ISOM_BOX_TYPE_HVT1:
		break;
	default:
		return GF_OK;
	}

	if (!entry->ipod_ext) {
		entry->ipod_ext = (GF_UnknownUUIDBox *) gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_UUID);
		if (!entry->ipod_ext) return GF_OUT_OF_MEM;
	}
	memcpy(entry->ipod_ext->uuid, GF_ISOM_IPOD_EXT, sizeof(u8)*16);
	entry->ipod_ext->dataSize = 4;
	entry->ipod_ext->data = gf_malloc(sizeof(u8)*4);
	if (!entry->ipod_ext->data) return GF_OUT_OF_MEM;
	memset(entry->ipod_ext->data, 0, sizeof(u8)*4);
	return GF_OK;
}