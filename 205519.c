TIFFjpeg_write_tables(JPEGState* sp)
{
	return CALLVJPEG(sp, jpeg_write_tables(&sp->cinfo.c));
}