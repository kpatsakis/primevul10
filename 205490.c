TIFFjpeg_set_defaults(JPEGState* sp)
{
	return CALLVJPEG(sp, jpeg_set_defaults(&sp->cinfo.c));
}