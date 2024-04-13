JPEGVSetField(TIFF* tif, uint32 tag, va_list ap)
{
	JPEGState* sp = JState(tif);
	const TIFFField* fip;
	uint32 v32;

	assert(sp != NULL);

	switch (tag) {
	case TIFFTAG_JPEGTABLES:
		v32 = (uint32) va_arg(ap, uint32);
		if (v32 == 0) {
			/* XXX */
			return (0);
		}
		_TIFFsetByteArray(&sp->jpegtables, va_arg(ap, void*), v32);
		sp->jpegtables_length = v32;
		TIFFSetFieldBit(tif, FIELD_JPEGTABLES);
		break;
	case TIFFTAG_JPEGQUALITY:
		sp->jpegquality = (int) va_arg(ap, int);
		return (1);			/* pseudo tag */
	case TIFFTAG_JPEGCOLORMODE:
		sp->jpegcolormode = (int) va_arg(ap, int);
		JPEGResetUpsampled( tif );
		return (1);			/* pseudo tag */
	case TIFFTAG_PHOTOMETRIC:
	{
		int ret_value = (*sp->vsetparent)(tif, tag, ap);
		JPEGResetUpsampled( tif );
		return ret_value;
	}
	case TIFFTAG_JPEGTABLESMODE:
		sp->jpegtablesmode = (int) va_arg(ap, int);
		return (1);			/* pseudo tag */
	case TIFFTAG_YCBCRSUBSAMPLING:
		/* mark the fact that we have a real ycbcrsubsampling! */
		sp->ycbcrsampling_fetched = 1;
		/* should we be recomputing upsampling info here? */
		return (*sp->vsetparent)(tif, tag, ap);
	default:
		return (*sp->vsetparent)(tif, tag, ap);
	}

	if ((fip = TIFFFieldWithTag(tif, tag)) != NULL) {
		TIFFSetFieldBit(tif, fip->field_bit);
	} else {
		return (0);
	}

	tif->tif_flags |= TIFF_DIRTYDIRECT;
	return (1);
}