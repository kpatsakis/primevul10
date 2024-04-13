GF_Err gf_isom_add_desc_to_description(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, const GF_Descriptor *theDesc)
{
	GF_IPIPtr *ipiD;
	GF_Err e;
	u16 tmpRef;
	GF_TrackBox *trak;
	GF_Descriptor *desc;
	GF_ESD *esd;
	GF_TrackReferenceBox *tref;
	GF_TrackReferenceTypeBox *dpnd;
	GF_MPEGVisualSampleEntryBox *entry;
	u32 msubtype;
	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	/*GETS NATIVE DESCRIPTOR ONLY*/
	e = Media_GetESD(trak->Media, StreamDescriptionIndex, &esd, GF_TRUE);
	if (e) return e;

	entry = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, StreamDescriptionIndex-1);
	if (!entry) return GF_BAD_PARAM;
	msubtype = entry->type;
	if ((msubtype==GF_ISOM_BOX_TYPE_ENCV) || (msubtype==GF_ISOM_BOX_TYPE_ENCA))
		gf_isom_get_original_format_type(movie, trackNumber, StreamDescriptionIndex, &msubtype);

	//duplicate the desc
	e = gf_odf_desc_copy((GF_Descriptor *)theDesc, &desc);
	if (e) return e;

	//and add it to the ESD EXCEPT IPI PTR (we need to translate from ES_ID to TrackID!!!
	if (!movie->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	switch (desc->tag) {
	case GF_ODF_IPI_PTR_TAG:
		goto insertIPI;
	default:
		break;
	}

	if ((msubtype==GF_ISOM_BOX_TYPE_MP4S) || (msubtype==GF_ISOM_BOX_TYPE_MP4V) || (msubtype==GF_ISOM_BOX_TYPE_MP4A)) {
		return gf_odf_desc_add_desc((GF_Descriptor *)esd, desc);
	}

	if (trak->Media->handler->handlerType!=GF_ISOM_MEDIA_VISUAL) {
		gf_odf_desc_del(desc);
		return GF_NOT_SUPPORTED;
	}
	GF_MPEG4ExtensionDescriptorsBox *mdesc = (GF_MPEG4ExtensionDescriptorsBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_M4DS);
	if (!mdesc) {
		mdesc = (GF_MPEG4ExtensionDescriptorsBox *) gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_M4DS);
	}
	return gf_list_add(mdesc->descriptors, desc);

insertIPI:
	if (esd->ipiPtr) {
		gf_odf_desc_del((GF_Descriptor *) esd->ipiPtr);
		esd->ipiPtr = NULL;
	}

	ipiD = (GF_IPIPtr *) desc;
	//find a tref
	if (!trak->References) {
		tref = (GF_TrackReferenceBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TREF);
		if (!tref) return GF_OUT_OF_MEM;
		e = trak_on_child_box((GF_Box*)trak, (GF_Box *)tref, GF_FALSE);
		if (e) return e;
	}
	tref = trak->References;

	e = Track_FindRef(trak, GF_ISOM_REF_IPI, &dpnd);
	if (e) return e;
	if (!dpnd) {
		tmpRef = 0;
		dpnd = (GF_TrackReferenceTypeBox *) gf_isom_box_new_parent(&tref->child_boxes, GF_ISOM_BOX_TYPE_REFT);
		if (!dpnd) return GF_OUT_OF_MEM;
		dpnd->reference_type = GF_ISOM_BOX_TYPE_IPIR;
		e = reftype_AddRefTrack(dpnd, ipiD->IPI_ES_Id, &tmpRef);
		if (e) return e;
		//and replace the tag and value...
		ipiD->IPI_ES_Id = tmpRef;
		ipiD->tag = GF_ODF_ISOM_IPI_PTR_TAG;
	} else {
		//Watch out! ONLY ONE IPI dependency is allowed per stream
		dpnd->trackIDCount = 1;
		dpnd->trackIDs[0] = ipiD->IPI_ES_Id;
		//and replace the tag and value...
		ipiD->IPI_ES_Id = 1;
		ipiD->tag = GF_ODF_ISOM_IPI_PTR_TAG;
	}
	//and add the desc to the esd...
	return gf_odf_desc_add_desc((GF_Descriptor *)esd, desc);
}