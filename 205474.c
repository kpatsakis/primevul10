TIFFjpeg_set_quality(JPEGState* sp, int quality, boolean force_baseline)
{
	return CALLVJPEG(sp,
	    jpeg_set_quality(&sp->cinfo.c, quality, force_baseline));
}