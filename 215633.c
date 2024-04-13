GF_Err AddMovieIOD(GF_MovieBox *moov, u8 isIOD)
{
	GF_Descriptor *od;
	GF_ObjectDescriptorBox *iods;

	//do we have an IOD ?? If not, create one.
	if (moov->iods) return GF_OK;

	if (isIOD) {
		od = gf_odf_desc_new(GF_ODF_ISOM_IOD_TAG);
	} else {
		od = gf_odf_desc_new(GF_ODF_ISOM_OD_TAG);
	}
	if (!od) return GF_OUT_OF_MEM;
	((GF_IsomObjectDescriptor *)od)->objectDescriptorID = 1;

	iods = (GF_ObjectDescriptorBox *) gf_isom_box_new_parent(&moov->child_boxes, GF_ISOM_BOX_TYPE_IODS);
	if (!iods) return GF_OUT_OF_MEM;
	iods->descriptor = od;
	return moov_on_child_box((GF_Box*)moov, (GF_Box *)iods, GF_FALSE);
}