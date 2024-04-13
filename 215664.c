GF_Err gf_isom_remove_track(GF_ISOFile *movie, u32 trackNumber)
{
	GF_Err e;
	GF_TrackBox *the_trak, *trak;
	GF_TrackReferenceTypeBox *tref;
	u32 i, j, k, descIndex;
	GF_ISOTrackID *newRefs;
	u8 found;
	GF_ISOSample *samp;
	the_trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!the_trak) return GF_BAD_PARAM;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (movie->moov->iods && movie->moov->iods->descriptor) {
		GF_Descriptor *desc;
		GF_ES_ID_Inc *inc;
		GF_List *ESDs;
		desc = movie->moov->iods->descriptor;
		if (desc->tag == GF_ODF_ISOM_IOD_TAG) {
			ESDs = ((GF_IsomInitialObjectDescriptor *)desc)->ES_ID_IncDescriptors;
		} else if (desc->tag == GF_ODF_ISOM_OD_TAG) {
			ESDs = ((GF_IsomObjectDescriptor *)desc)->ES_ID_IncDescriptors;
		} else {
			return GF_ISOM_INVALID_FILE;
		}

		//remove the track ref from the root OD if any
		i=0;
		while ((inc = (GF_ES_ID_Inc *)gf_list_enum(ESDs, &i))) {
			if (inc->trackID == the_trak->Header->trackID) {
				gf_odf_desc_del((GF_Descriptor *)inc);
				i--;
				gf_list_rem(ESDs, i);
			}
		}
	}

	//remove the track from the movie
	gf_list_del_item(movie->moov->trackList, the_trak);

	//rewrite any OD tracks
	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(movie->moov->trackList, &i))) {
		if (trak->Media->handler->handlerType != GF_ISOM_MEDIA_OD) continue;
		//this is an OD track...
		j = gf_isom_get_sample_count(movie, i);
		for (k=0; k < j; k++) {
			//getting the sample will remove the references to the deleted track in the output OD frame
			samp = gf_isom_get_sample(movie, i, k+1, &descIndex);
			if (!samp) break;
			//so let's update with the new OD frame ! If the sample is empty, remove it
			if (!samp->dataLength) {
				e = gf_isom_remove_sample(movie, i, k+1);
				if (e) return e;
			} else {
				e = gf_isom_update_sample(movie, i, k+1, samp, GF_TRUE);
				if (e) return e;
			}
			//and don't forget to delete the sample
			gf_isom_sample_del(&samp);
		}
	}

	//remove the track ref from any "tref" box in all tracks, except the one to delete
	//note that we don't touch scal references, as we don't want to rewrite AVC/HEVC samples ...
	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(movie->moov->trackList, &i))) {
		if (trak == the_trak) continue;
		if (! trak->References || ! gf_list_count(trak->References->child_boxes)) continue;

		j=0;
		while ((tref = (GF_TrackReferenceTypeBox *)gf_list_enum(trak->References->child_boxes, &j))) {
			if (tref->reference_type==GF_ISOM_REF_SCAL)
				continue;

			found = 0;
			for (k=0; k<tref->trackIDCount; k++) {
				if (tref->trackIDs[k] == the_trak->Header->trackID) found++;
			}
			if (!found) continue;
			//no more refs, remove this ref_type
			if (found == tref->trackIDCount) {
				gf_isom_box_del_parent(&trak->References->child_boxes, (GF_Box *)tref);
				j--;
			} else {
				newRefs = (GF_ISOTrackID*)gf_malloc(sizeof(GF_ISOTrackID) * (tref->trackIDCount - found));
				if (!newRefs) return GF_OUT_OF_MEM;
				found = 0;
				for (k = 0; k < tref->trackIDCount; k++) {
					if (tref->trackIDs[k] != the_trak->Header->trackID) {
						newRefs[k-found] = tref->trackIDs[k];
					} else {
						found++;
					}
				}
				gf_free(tref->trackIDs);
				tref->trackIDs = newRefs;
				tref->trackIDCount -= found;
			}
		}
		//a little opt: remove the ref box if empty...
		if (! gf_list_count(trak->References->child_boxes)) {
			gf_isom_box_del_parent(&trak->child_boxes, (GF_Box *)trak->References);
			trak->References = NULL;
		}
	}

	gf_isom_disable_inplace_rewrite(movie);

	//delete the track
	gf_isom_box_del_parent(&movie->moov->child_boxes, (GF_Box *)the_trak);

	/*update next track ID*/
	movie->moov->mvhd->nextTrackID = 0;
	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(movie->moov->trackList, &i))) {
		if (trak->Header->trackID>movie->moov->mvhd->nextTrackID)
			movie->moov->mvhd->nextTrackID = trak->Header->trackID;
	}

	if (!gf_list_count(movie->moov->trackList)) {
		gf_list_del_item(movie->TopBoxes, movie->moov);
		gf_isom_box_del((GF_Box *)movie->moov);
		movie->moov = NULL;
	}
	return GF_OK;
}