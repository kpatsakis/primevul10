GF_Err gf_isom_clone_root_od(GF_ISOFile *input, GF_ISOFile *output)
{
	GF_List *esds;
	GF_Err e;
	u32 i;
	GF_Descriptor *desc;

	e = gf_isom_remove_root_od(output);
	if (e) return e;
	if (!input->moov || !input->moov->iods || !input->moov->iods->descriptor) return GF_OK;
	e = gf_isom_insert_moov(output);
	if (e) return e;
	e = AddMovieIOD(output->moov, 0);
	if (e) return e;
	if (output->moov->iods->descriptor) gf_odf_desc_del(output->moov->iods->descriptor);
	output->moov->iods->descriptor = NULL;
	gf_odf_desc_copy(input->moov->iods->descriptor, &output->moov->iods->descriptor);

	switch (output->moov->iods->descriptor->tag) {
	case GF_ODF_ISOM_IOD_TAG:
		esds = ((GF_IsomInitialObjectDescriptor *)output->moov->iods->descriptor)->ES_ID_IncDescriptors;
		break;
	case GF_ODF_ISOM_OD_TAG:
		esds = ((GF_IsomObjectDescriptor *)output->moov->iods->descriptor)->ES_ID_IncDescriptors;
		break;
	default:
		return GF_ISOM_INVALID_FILE;
	}

	//get the desc
	i=0;
	while ((desc = (GF_Descriptor*)gf_list_enum(esds, &i))) {
		gf_odf_desc_del(desc);
		gf_list_rem(esds, i-1);
	}
	return GF_OK;
}