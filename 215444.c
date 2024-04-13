
GF_Err jp2h_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_J2KHeaderBox *ptr = (GF_J2KHeaderBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_IHDR:
		if (ptr->ihdr) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->ihdr = (GF_J2KImageHeaderBox*)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_COLR:
		if (ptr->colr) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->colr = (GF_ColourInformationBox*)a;
		return GF_OK;
	}
	return GF_OK;