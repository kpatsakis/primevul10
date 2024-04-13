static GF_Err gf_isom_set_root_iod(GF_ISOFile *movie)
{
	GF_IsomInitialObjectDescriptor *iod;
	GF_IsomObjectDescriptor *od;
	GF_Err e;
	
	e = gf_isom_insert_moov(movie);
	if (e) return e;
	if (!movie->moov->iods) {
		AddMovieIOD(movie->moov, 1);
		return GF_OK;
	}
	//if OD, switch to IOD
	if (movie->moov->iods->descriptor->tag == GF_ODF_ISOM_IOD_TAG) return GF_OK;
	od = (GF_IsomObjectDescriptor *) movie->moov->iods->descriptor;
	iod = (GF_IsomInitialObjectDescriptor*)gf_malloc(sizeof(GF_IsomInitialObjectDescriptor));
	if (!iod) return GF_OUT_OF_MEM;

	memset(iod, 0, sizeof(GF_IsomInitialObjectDescriptor));

	iod->ES_ID_IncDescriptors = od->ES_ID_IncDescriptors;
	od->ES_ID_IncDescriptors = NULL;
	//not used in root OD
	iod->ES_ID_RefDescriptors = NULL;
	iod->extensionDescriptors = od->extensionDescriptors;
	od->extensionDescriptors = NULL;
	iod->IPMP_Descriptors = od->IPMP_Descriptors;
	od->IPMP_Descriptors = NULL;
	iod->objectDescriptorID = od->objectDescriptorID;
	iod->OCIDescriptors = od->OCIDescriptors;
	od->OCIDescriptors = NULL;
	iod->tag = GF_ODF_ISOM_IOD_TAG;
	iod->URLString = od->URLString;
	od->URLString = NULL;

	gf_odf_desc_del((GF_Descriptor *) od);
	movie->moov->iods->descriptor = (GF_Descriptor *)iod;
	return GF_OK;
}