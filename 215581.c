GF_Err gf_isom_clone_pl_indications(GF_ISOFile *orig, GF_ISOFile *dest)
{
	GF_IsomInitialObjectDescriptor *iod_d;
	if (!orig || !dest) return GF_BAD_PARAM;
	if (!orig->moov->iods || !orig->moov->iods->descriptor) return GF_OK;
	if (orig->moov->iods->descriptor->tag != GF_ODF_ISOM_IOD_TAG) return GF_OK;

	AddMovieIOD(dest->moov, 1);
	gf_odf_desc_del((GF_Descriptor *)dest->moov->iods->descriptor);
	gf_odf_desc_copy((GF_Descriptor *)orig->moov->iods->descriptor, (GF_Descriptor **)&dest->moov->iods->descriptor);
	iod_d = (GF_IsomInitialObjectDescriptor *) dest->moov->iods->descriptor;
	while (gf_list_count(iod_d->ES_ID_IncDescriptors)) {
		GF_Descriptor *d = (GF_Descriptor *)gf_list_get(iod_d->ES_ID_IncDescriptors, 0);
		gf_list_rem(iod_d->ES_ID_IncDescriptors, 0);
		gf_odf_desc_del(d);
	}
	while (gf_list_count(iod_d->ES_ID_RefDescriptors)) {
		GF_Descriptor *d = (GF_Descriptor *)gf_list_get(iod_d->ES_ID_RefDescriptors, 0);
		gf_list_rem(iod_d->ES_ID_RefDescriptors, 0);
		gf_odf_desc_del(d);
	}
	return GF_OK;
}