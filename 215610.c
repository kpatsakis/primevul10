GF_Err gf_isom_set_track_switch_parameter(GF_ISOFile *movie, u32 trackNumber, u32 trackRefGroup, Bool is_switch_group, u32 *switchGroupID, u32 *criteriaList, u32 criteriaListCount)
{
	GF_TrackSelectionBox *tsel;
	GF_TrackBox *trak;
	GF_UserDataMap *map;
	GF_Err e;
	u32 alternateGroupID = 0;
	u32 next_switch_group_id = 0;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !switchGroupID) return GF_BAD_PARAM;


	if (trackRefGroup) {
		GF_TrackBox *trak_ref = gf_isom_get_track_from_file(movie, trackRefGroup);
		if (trak_ref != trak) {
			if (!trak_ref || !trak_ref->Header->alternate_group) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("Track %d has not an alternate group - skipping\n", trak_ref ? trak_ref->Header->trackID : 0));
				return GF_BAD_PARAM;
			}
			alternateGroupID = trak_ref->Header->alternate_group;
		} else {
			alternateGroupID = trak->Header->alternate_group;
		}
	}
	if (!alternateGroupID) {
		/*there is a function for this ....*/
		if (trak->Header->alternate_group) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("Track %d has already an alternate group - skipping\n", trak->Header->trackID));
			return GF_BAD_PARAM;
		}
		alternateGroupID = gf_isom_get_next_alternate_group_id(movie);
	}

	if (is_switch_group) {
		u32 i=0;
		while (i< gf_isom_get_track_count(movie) ) {
			//locate first available ID
			GF_TrackBox *a_trak = gf_isom_get_track_from_file(movie, i+1);

			if (a_trak->udta) {
				u32 j, count;
				map = udta_getEntry(a_trak->udta, GF_ISOM_BOX_TYPE_TSEL, NULL);
				if (map) {
					count = gf_list_count(map->boxes);
					for (j=0; j<count; j++) {
						tsel = (GF_TrackSelectionBox*)gf_list_get(map->boxes, j);

						if (*switchGroupID) {
							if (tsel->switchGroup==next_switch_group_id) {
								if (a_trak->Header->alternate_group != alternateGroupID) return GF_BAD_PARAM;
							}
						} else {
							if (tsel->switchGroup && (tsel->switchGroup>=next_switch_group_id) )
								next_switch_group_id = tsel->switchGroup;
						}
					}
				}

			}
			i++;
		}
		if (! *switchGroupID) *switchGroupID = next_switch_group_id+1;
	}


	trak->Header->alternate_group = alternateGroupID;

	tsel = NULL;
	if (*switchGroupID) {
		if (!trak->udta) {
			e = trak_on_child_box((GF_Box*)trak, gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
			if (e) return e;
		}

		map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_TSEL, NULL);

		/*locate tsel box with no switch group*/
		if (map)  {
			u32 j, count = gf_list_count(map->boxes);
			for (j=0; j<count; j++) {
				tsel = (GF_TrackSelectionBox*)gf_list_get(map->boxes, j);
				if (tsel->switchGroup == *switchGroupID) break;
				tsel = NULL;
			}
		}
		if (!tsel) {
			tsel = (GF_TrackSelectionBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_TSEL);
			if (!tsel) return GF_OUT_OF_MEM;
			e = udta_on_child_box((GF_Box *)trak->udta, (GF_Box *) tsel, GF_FALSE);
			if (e) return e;
		}

		tsel->switchGroup = *switchGroupID;
		tsel->attributeListCount = criteriaListCount;
		if (tsel->attributeList) gf_free(tsel->attributeList);
		tsel->attributeList = (u32*)gf_malloc(sizeof(u32)*criteriaListCount);
		if (!tsel->attributeList) return GF_OUT_OF_MEM;
		memcpy(tsel->attributeList, criteriaList, sizeof(u32)*criteriaListCount);
	}
	return GF_OK;
}