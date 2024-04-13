TIFFjpeg_write_raw_data(JPEGState* sp, JSAMPIMAGE data, int num_lines)
{
	return CALLJPEG(sp, -1, (int) jpeg_write_raw_data(&sp->cinfo.c,
	    data, (JDIMENSION) num_lines));
}