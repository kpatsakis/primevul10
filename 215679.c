u64 gf_isom_estimate_size(GF_ISOFile *movie)
{
	GF_Err e;
	GF_Box *a;
	u32 i, count;
	u64 mdat_size;
	if (!movie || !movie->moov) return 0;

	mdat_size = 0;
	count = gf_list_count(movie->moov->trackList);
	for (i=0; i<count; i++) {
		mdat_size += gf_isom_get_media_data_size(movie, i+1);
	}
	if (mdat_size) {
		mdat_size += 8;
		if (mdat_size > 0xFFFFFFFF) mdat_size += 8;
	}

	i=0;
	while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
		e = gf_isom_box_size(a);
		if (e == GF_OK)
			mdat_size += a->size;
	}
	return mdat_size;
}