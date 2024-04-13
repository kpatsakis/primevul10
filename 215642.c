GF_Err gf_isom_reset_alt_brands_ex(GF_ISOFile *movie, Bool leave_empty)
{
	u32 *p;

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (! (movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY)) {
		GF_Err e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
		if (e) return e;

		e = CheckNoData(movie);
		if (e) return e;
	}
#endif

	if (!movie->brand) {
		movie->brand = (GF_FileTypeBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_FTYP);
		if (!movie->brand) return GF_OUT_OF_MEM;
		gf_list_add(movie->TopBoxes, movie->brand);
	}
	gf_free(movie->brand->altBrand);
	if (leave_empty) {
		movie->brand->altCount = 0;
		movie->brand->altBrand = NULL;
	} else {
		p = (u32*)gf_malloc(sizeof(u32));
		if (!p) return GF_OUT_OF_MEM;
		p[0] = movie->brand->majorBrand;
		movie->brand->altCount = 1;
		movie->brand->altBrand = p;
	}
	return GF_OK;
}