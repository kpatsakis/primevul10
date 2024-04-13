TIFFjpeg_read_scanlines(JPEGState* sp, JSAMPARRAY scanlines, int max_lines)
{
	return CALLJPEG(sp, -1, (int) jpeg_read_scanlines(&sp->cinfo.d,
	    scanlines, (JDIMENSION) max_lines));
}