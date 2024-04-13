TIFFjpeg_suppress_tables(JPEGState* sp, boolean suppress)
{
	return CALLVJPEG(sp, jpeg_suppress_tables(&sp->cinfo.c, suppress));
}