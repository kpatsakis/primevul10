GF_EXPORT
GF_Err gf_isom_release_segment(GF_ISOFile *movie, Bool reset_tables)
{
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	u32 i, j, base_track_sample_count;
	Bool has_scalable;
	GF_Box *a;
	if (!movie || !movie->moov || !movie->moov->mvex) return GF_BAD_PARAM;
	has_scalable = gf_isom_needs_layer_reconstruction(movie);
	base_track_sample_count = 0;
	movie->moov->compressed_diff = 0;
	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
		trak->first_traf_merged = GF_FALSE;
		if (trak->Media->information->dataHandler == movie->movieFileMap) {
			trak->Media->information->dataHandler = NULL;
		}
		if (trak->Media->information->scalableDataHandler == movie->movieFileMap) {
			trak->Media->information->scalableDataHandler = NULL;
		} else {
			if (trak->Media->information->scalableDataHandler==trak->Media->information->dataHandler)
				trak->Media->information->dataHandler = NULL;

			gf_isom_datamap_del(trak->Media->information->scalableDataHandler);
			trak->Media->information->scalableDataHandler = NULL;
		}


		if (reset_tables) {
			u32 type, dur;
			u64 dts;
			GF_SampleTableBox *stbl = trak->Media->information->sampleTable;

			if (has_scalable) {
				//check if the base reference is in the file - if not, do not consider the track is scalable.
				if (gf_isom_get_reference_count(movie, i+1, GF_ISOM_REF_BASE) > 0) {
					u32 on_track=0;
					GF_TrackBox *base;
					gf_isom_get_reference(movie, i+1, GF_ISOM_REF_BASE, 1, &on_track);

					base = gf_isom_get_track_from_file(movie, on_track);
					if (!base) {
						base_track_sample_count=0;
					} else {
						base_track_sample_count = base->Media->information->sampleTable->SampleSize->sampleCount;
					}
				}
			}

			trak->sample_count_at_seg_start += base_track_sample_count ? base_track_sample_count : stbl->SampleSize->sampleCount;

			if (trak->sample_count_at_seg_start) {
				GF_Err e;
				e = stbl_GetSampleDTS_and_Duration(stbl->TimeToSample, stbl->SampleSize->sampleCount, &dts, &dur);
				if (e == GF_OK) {
					trak->dts_at_seg_start += dts + dur;
				}
			}

			RECREATE_BOX(stbl->ChunkOffset, (GF_Box *));
			RECREATE_BOX(stbl->CompositionOffset, (GF_CompositionOffsetBox *));
			RECREATE_BOX(stbl->DegradationPriority, (GF_DegradationPriorityBox *));
			RECREATE_BOX(stbl->PaddingBits, (GF_PaddingBitsBox *));
			RECREATE_BOX(stbl->SampleDep, (GF_SampleDependencyTypeBox *));
			RECREATE_BOX(stbl->SampleSize, (GF_SampleSizeBox *));
			RECREATE_BOX(stbl->SampleToChunk, (GF_SampleToChunkBox *));
			RECREATE_BOX(stbl->ShadowSync, (GF_ShadowSyncBox *));
			RECREATE_BOX(stbl->SyncSample, (GF_SyncSampleBox *));
			RECREATE_BOX(stbl->TimeToSample, (GF_TimeToSampleBox *));

			gf_isom_box_array_del_parent(&stbl->child_boxes, stbl->sai_offsets);
			stbl->sai_offsets = NULL;

			gf_isom_box_array_del_parent(&stbl->child_boxes, stbl->sai_sizes);
			stbl->sai_sizes = NULL;

			gf_isom_box_array_del_parent(&stbl->child_boxes, stbl->sampleGroups);
			stbl->sampleGroups = NULL;

			if (trak->sample_encryption) {
				if (trak->Media->information->sampleTable->child_boxes) {
					gf_list_del_item(trak->Media->information->sampleTable->child_boxes, trak->sample_encryption);
				}
				gf_isom_box_del_parent(&trak->child_boxes, (GF_Box*)trak->sample_encryption);
				trak->sample_encryption = NULL;
			}

			j = stbl->nb_sgpd_in_stbl;
			while ((a = (GF_Box *)gf_list_enum(stbl->sampleGroupsDescription, &j))) {
				gf_isom_box_del_parent(&stbl->child_boxes, a);
				j--;
				gf_list_rem(stbl->sampleGroupsDescription, j);
			}

			if (stbl->traf_map) {
				for (j=0; j<stbl->traf_map->nb_entries; j++) {
					if (stbl->traf_map->frag_starts[j].moof_template)
						gf_free(stbl->traf_map->frag_starts[j].moof_template);
				}
				memset(stbl->traf_map->frag_starts, 0, sizeof(GF_TrafMapEntry)*stbl->traf_map->nb_alloc);
				stbl->traf_map->nb_entries = 0;
			}

#if 0 // TO CHECK
			j = ptr->nb_stbl_boxes;
			while ((a = (GF_Box *)gf_list_enum(stbl->child_boxes, &j))) {
				gf_isom_box_del_parent(&stbl->child_boxes, a);
				j--;
			}
#endif
		}


		j = 0;
		while ((a = (GF_Box *)gf_list_enum(movie->moov->child_boxes, &j))) {
			if (a->type == GF_ISOM_BOX_TYPE_PSSH) {
				gf_isom_box_del_parent(&movie->moov->child_boxes, a);
				j--;
			}
		}
	}

	gf_isom_datamap_del(movie->movieFileMap);
	movie->movieFileMap = NULL;
#endif
	return GF_OK;