GF_Err gf_isom_clone_movie(GF_ISOFile *orig_file, GF_ISOFile *dest_file, Bool clone_tracks, Bool keep_hint_tracks, Bool keep_pssh)
{
	GF_Err e;
	u32 i;
	GF_Box *box;

	e = CanAccessMovie(dest_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	if (orig_file->brand) {
		gf_list_del_item(dest_file->TopBoxes, dest_file->brand);
		gf_isom_box_del((GF_Box *)dest_file->brand);
		dest_file->brand = NULL;
		gf_isom_clone_box((GF_Box *)orig_file->brand, (GF_Box **)&dest_file->brand);
		if (dest_file->brand) gf_list_add(dest_file->TopBoxes, dest_file->brand);
	}

	if (orig_file->meta) {
		gf_list_del_item(dest_file->TopBoxes, dest_file->meta);
		gf_isom_box_del((GF_Box *)dest_file->meta);
		dest_file->meta = NULL;
		/*fixme - check imports*/
		gf_isom_clone_box((GF_Box *)orig_file->meta, (GF_Box **)&dest_file->meta);
		if (dest_file->meta) gf_list_add(dest_file->TopBoxes, dest_file->meta);
	}
	if (orig_file->moov) {
		u32 i, dstTrack;
		GF_Box *iods;
		GF_List *tracks = gf_list_new();
		GF_List *old_tracks = orig_file->moov->trackList;
		orig_file->moov->trackList = tracks;
		iods = (GF_Box*)orig_file->moov->iods;
		orig_file->moov->iods = NULL;
		e = gf_isom_clone_box((GF_Box *)orig_file->moov, (GF_Box **)&dest_file->moov);
		if (e) {
			gf_list_del(tracks);
			orig_file->moov->trackList = old_tracks;
			return e;
		}
		orig_file->moov->trackList = old_tracks;
		gf_list_del(tracks);
		orig_file->moov->iods = (GF_ObjectDescriptorBox*)iods;
		gf_list_add(dest_file->TopBoxes, dest_file->moov);

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
		if (dest_file->moov->mvex) {
			gf_isom_box_del_parent(&dest_file->moov->child_boxes, (GF_Box *)dest_file->moov->mvex);
			dest_file->moov->mvex = NULL;
		}
#endif

		if (clone_tracks) {
			for (i=0; i<gf_list_count(orig_file->moov->trackList); i++) {
				GF_TrackBox *trak = (GF_TrackBox*)gf_list_get( orig_file->moov->trackList, i);
				if (!trak) continue;
				if (keep_hint_tracks || (trak->Media->handler->handlerType != GF_ISOM_MEDIA_HINT)) {
					e = gf_isom_clone_track(orig_file, i+1, dest_file, 0, &dstTrack);
					if (e) return e;
				}
			}
			if (iods)
				gf_isom_clone_box((GF_Box *)orig_file->moov->iods, (GF_Box **)dest_file->moov->iods);
		} else {
			dest_file->moov->mvhd->nextTrackID = 1;
			gf_isom_clone_pl_indications(orig_file, dest_file);
		}
		dest_file->moov->mov = dest_file;
	}

	if (!keep_pssh) {
		i=0;
		while ((box = (GF_Box*)gf_list_get(dest_file->moov->child_boxes, i++))) {
			if (box->type == GF_ISOM_BOX_TYPE_PSSH) {
				i--;
				gf_isom_box_del_parent(&dest_file->moov->child_boxes, box);
			}
		}
	}

	//duplicate other boxes
	i=0;
	while ((box = (GF_Box*)gf_list_get(orig_file->TopBoxes, i++))) {
		switch(box->type) {
		case GF_ISOM_BOX_TYPE_MOOV:
		case GF_ISOM_BOX_TYPE_META:
		case GF_ISOM_BOX_TYPE_MDAT:
		case GF_ISOM_BOX_TYPE_FTYP:
		case GF_ISOM_BOX_TYPE_PDIN:
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
		case GF_ISOM_BOX_TYPE_STYP:
		case GF_ISOM_BOX_TYPE_SIDX:
		case GF_ISOM_BOX_TYPE_SSIX:
		case GF_ISOM_BOX_TYPE_MOOF:
#endif
		case GF_ISOM_BOX_TYPE_JP:
			break;

		case GF_ISOM_BOX_TYPE_PSSH:
			if (!keep_pssh)
				break;

		default:
		{
			GF_Box *box2 = NULL;
			gf_isom_clone_box(box, &box2);
			gf_list_add(dest_file->TopBoxes, box2);
		}
		break;
		}
	}

	return GF_OK;
}