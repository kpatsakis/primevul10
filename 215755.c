GF_Err gf_isom_modify_alternate_brand(GF_ISOFile *movie, u32 Brand, Bool AddIt)
{
	u32 i, k, *p;

	if (!Brand) return GF_BAD_PARAM;

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (! (movie->FragmentsFlags & GF_ISOM_FRAG_WRITE_READY)) {
		GF_Err e = CanAccessMovie(movie, GF_ISOM_OPEN_WRITE);
		if (e) return e;

		e = CheckNoData(movie);
		if (e) return e;
	}
#endif

	if (!movie->brand && AddIt) {
		movie->brand = (GF_FileTypeBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_FTYP);
		if (!movie->brand) return GF_OUT_OF_MEM;
		gf_list_add(movie->TopBoxes, movie->brand);
	}
	if (!AddIt && !movie->brand) return GF_OK;

	//do not mofify major one
	if (!AddIt && movie->brand->majorBrand == Brand) return GF_OK;

	if (!AddIt && movie->brand->altCount == 1) {
		//fixes it in case
		movie->brand->altBrand[0] = movie->brand->majorBrand;
		return GF_OK;
	}
	//check for the brand
	for (i=0; i<movie->brand->altCount; i++) {
		if (movie->brand->altBrand[i] == Brand) goto found;
	}
	//Not found
	if (!AddIt) return GF_OK;
	//add it
	p = (u32*)gf_malloc(sizeof(u32)*(movie->brand->altCount + 1));
	if (!p) return GF_OUT_OF_MEM;
	if (movie->brand->altBrand) {
		memcpy(p, movie->brand->altBrand, sizeof(u32)*movie->brand->altCount);
		gf_free(movie->brand->altBrand);
	}
	p[movie->brand->altCount] = Brand;
	movie->brand->altCount += 1;
	movie->brand->altBrand = p;
	return GF_OK;

found:

	//found
	if (AddIt) return GF_OK;
	assert(movie->brand->altCount>1);

	//remove it
	p = (u32*)gf_malloc(sizeof(u32)*(movie->brand->altCount - 1));
	if (!p) return GF_OUT_OF_MEM;
	k = 0;
	for (i=0; i<movie->brand->altCount; i++) {
		if (movie->brand->altBrand[i] == Brand) continue;
		else {
			p[k] = movie->brand->altBrand[i];
			k++;
		}
	}
	movie->brand->altCount -= 1;
	gf_free(movie->brand->altBrand);
	movie->brand->altBrand = p;
	return GF_OK;
}