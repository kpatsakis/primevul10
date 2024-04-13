
GF_Err metx_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MetaDataSampleEntryBox *ptr = (GF_MetaDataSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_SINF:
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TXTC:
		//we allow the config box on metx
		if (ptr->config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->config = (GF_TextConfigBox *)a;
		break;
	}
	return GF_OK;