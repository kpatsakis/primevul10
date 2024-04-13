GF_Err gf_isom_set_track_index(GF_ISOFile *movie, u32 trackNumber, u32 index, void (*track_num_changed)(void *udta, u32 old_track_num, u32 new_track_num), void *udta)
{
	u32 i, count;
	GF_List *tracks;
	u32 prev_index=0, prev_pos=0;
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !index) return GF_BAD_PARAM;
	trak->index = index;
	tracks = gf_list_new();
	count = gf_list_count(movie->moov->trackList);
	//sort tracks in new list
	for (i=0; i<count; i++) {
		GF_TrackBox *a_tk = gf_list_get(movie->moov->trackList, i);
		if (!a_tk->index) {
			gf_list_insert(tracks, a_tk, 0);
		} else if (a_tk->index < prev_index) {
			gf_list_insert(tracks, a_tk, prev_pos);
		} else {
			gf_list_add(tracks, a_tk);
		}
		prev_pos = gf_list_count(tracks) - 1;
		prev_index = a_tk->index;
	}
	if (gf_list_count(tracks) != count) {
		gf_list_del(tracks);
		return GF_OUT_OF_MEM;
	}
	if (track_num_changed) {
		for (i=0; i<count; i++) {
			GF_TrackBox *a_tk = gf_list_get(tracks, i);
			s32 old_pos = gf_list_find(movie->moov->trackList, a_tk);
			assert(old_pos>=0);
			if (old_pos != i)
				track_num_changed(udta, old_pos+1, i+1);
		}
	}
	gf_list_del(movie->moov->trackList);
	movie->moov->trackList = tracks;
	return GF_OK;
}