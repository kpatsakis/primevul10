u32 gf_isom_new_track_from_template(GF_ISOFile *movie, GF_ISOTrackID trakID, u32 MediaType, u32 TimeScale, u8 *tk_box, u32 tk_box_size, Bool udta_only)
{
	GF_Err e;
	u64 now;
	u8 isHint;
	GF_TrackBox *trak;
	GF_TrackHeaderBox *tkhd;
	GF_MediaBox *mdia;
	GF_UserDataBox *udta = NULL;

	e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
	if (e) {
		gf_isom_set_last_error(movie, e);
		return 0;
	}
	e = gf_isom_insert_moov(movie);
	if (e) return e;


	isHint = 0;
	//we're creating a hint track... it's the same, but mode HAS TO BE EDIT
	if (MediaType == GF_ISOM_MEDIA_HINT) {
//		if (movie->openMode != GF_ISOM_OPEN_EDIT) return 0;
		isHint = 1;
	}

	mdia = NULL;
	tkhd = NULL;
	trak = NULL;
	if (trakID) {
		//check if we are in ES_ID boundaries
		if (!isHint && (trakID > 0xFFFF)) {
			gf_isom_set_last_error(movie, GF_BAD_PARAM);
			return 0;
		}
		//here we should look for available IDs ...
		if (!RequestTrack(movie->moov, trakID)) return 0;
	} else {
		trakID = movie->moov->mvhd->nextTrackID;
		if (!trakID) trakID = 1;
		/*ESIDs are on 16 bits*/
		if (! isHint && (trakID > 0xFFFF)) trakID = 1;

		while (1) {
			if (RequestTrack(movie->moov, trakID)) break;
			trakID += 1;
			if (trakID == 0xFFFFFFFF) break;
		}
		if (trakID == 0xFFFFFFFF) {
			gf_isom_set_last_error(movie, GF_BAD_PARAM);
			return 0;
		}
		if (! isHint && (trakID > 0xFFFF)) {
			gf_isom_set_last_error(movie, GF_BAD_PARAM);
			return 0;
		}
	}

	if (tk_box) {
		GF_BitStream *bs = gf_bs_new(tk_box, tk_box_size, GF_BITSTREAM_READ);
		gf_bs_set_cookie(bs, GF_ISOM_BS_COOKIE_NO_LOGS|GF_ISOM_BS_COOKIE_CLONE_TRACK);

		e = gf_isom_box_parse_ex((GF_Box**)&trak, bs, GF_ISOM_BOX_TYPE_MOOV, GF_FALSE);
		gf_bs_del(bs);
		if (e) trak = NULL;
		else if (udta_only) {
			udta = trak->udta;
			trak->udta = NULL;
			gf_isom_box_del((GF_Box*)trak);
		} else {
			Bool tpl_ok = GF_TRUE;
			if (!trak->Header || !trak->Media || !trak->Media->handler || !trak->Media->mediaHeader || !trak->Media->information) tpl_ok = GF_FALSE;

			else {
				if (!MediaType) MediaType = trak->Media->handler->handlerType;
				e = NewMedia(&trak->Media, MediaType, TimeScale);
				if (e) tpl_ok = GF_FALSE;
			}
			if (!tpl_ok) {
				udta = trak->udta;
				trak->udta = NULL;
				gf_isom_box_del((GF_Box*)trak);
			}
		}
	}
	now = gf_isom_get_mp4time();
	if (!trak) {
		//OK, now create a track...
		trak = (GF_TrackBox *) gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_TRAK);
		if (!trak) {
			gf_isom_set_last_error(movie, GF_OUT_OF_MEM);
			return 0;
		}
		tkhd = (GF_TrackHeaderBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TKHD);
		if (!tkhd) {
			gf_isom_set_last_error(movie, GF_OUT_OF_MEM);
			return 0;
		}

		//OK, set up the media trak
		e = NewMedia(&mdia, MediaType, TimeScale);
		if (e) {
			gf_isom_box_del((GF_Box *)mdia);
			return 0;
		}
		assert(trak->child_boxes);
		gf_list_add(trak->child_boxes, mdia);

		//OK, add this media to our track
		mdia->mediaTrack = trak;

		e = trak_on_child_box((GF_Box*)trak, (GF_Box *) tkhd, GF_FALSE);
		if (e) goto err_exit;
		e = trak_on_child_box((GF_Box*)trak, (GF_Box *) mdia, GF_FALSE);
		if (e) goto err_exit;
		tkhd->trackID = trakID;

		if (gf_sys_is_test_mode() ) {
			tkhd->creationTime = 0;
			mdia->mediaHeader->creationTime = 0;
		} else {
			tkhd->creationTime = now;
			mdia->mediaHeader->creationTime = now;
		}

	} else {
		tkhd = trak->Header;
		tkhd->trackID = trakID;
		mdia = trak->Media;
		mdia->mediaTrack = trak;
		mdia->mediaHeader->timeScale = TimeScale;
		if (mdia->handler->handlerType != MediaType) {
			mdia->handler->handlerType = MediaType;
			tkhd->width = 0;
			tkhd->height = 0;
			tkhd->volume = 0;
		} else {
			MediaType = 0;
		}
		trak->Header->duration = 0;
		mdia->mediaHeader->duration = 0;

		if (!movie->moov->child_boxes) movie->moov->child_boxes = gf_list_new();
		gf_list_add(movie->moov->child_boxes, trak);
	}
	if (MediaType) {
		//some default properties for Audio, Visual or private tracks
		switch (MediaType) {
		case GF_ISOM_MEDIA_VISUAL:
		case GF_ISOM_MEDIA_AUXV:
		case GF_ISOM_MEDIA_PICT:
		case GF_ISOM_MEDIA_SCENE:
		case GF_ISOM_MEDIA_TEXT:
		case GF_ISOM_MEDIA_SUBT:
			/*320-240 pix in 16.16*/
			tkhd->width = 0x01400000;
			tkhd->height = 0x00F00000;
			break;
		case GF_ISOM_MEDIA_AUDIO:
			tkhd->volume = 0x0100;
			break;
		}
	}
	movie->last_created_track_id = tkhd->trackID;
	
	if (!movie->keep_utc && !gf_sys_is_test_mode() ) {
		tkhd->modificationTime = now;
	 	mdia->mediaHeader->modificationTime = now;
	}

	//OK, add our trak
	e = moov_on_child_box((GF_Box*)movie->moov, (GF_Box *)trak, GF_FALSE);
	if (e) goto err_exit;
	//set the new ID available
	if (trakID+1> movie->moov->mvhd->nextTrackID)
		movie->moov->mvhd->nextTrackID = trakID+1;

	trak->udta = udta;

	//and return our track number
	return gf_isom_get_track_by_id(movie, trakID);

err_exit:
	//tkhd is registered with track and will be destroyed there
	if (trak) gf_isom_box_del((GF_Box *)trak);
	if (mdia) gf_isom_box_del((GF_Box *)mdia);
	return 0;
}