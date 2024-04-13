TIFFjpeg_write_scanlines(JPEGState* sp, JSAMPARRAY scanlines, int num_lines)
{
	return CALLJPEG(sp, -1, (int) jpeg_write_scanlines(&sp->cinfo.c,
	    scanlines, (JDIMENSION) num_lines));
}