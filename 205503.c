TIFFjpeg_start_compress(JPEGState* sp, boolean write_all_tables)
{
	return CALLVJPEG(sp,
	    jpeg_start_compress(&sp->cinfo.c, write_all_tables));
}