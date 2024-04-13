TIFFjpeg_set_colorspace(JPEGState* sp, J_COLOR_SPACE colorspace)
{
	return CALLVJPEG(sp, jpeg_set_colorspace(&sp->cinfo.c, colorspace));
}