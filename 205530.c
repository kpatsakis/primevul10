TIFFjpeg_read_raw_data(JPEGState* sp, JSAMPIMAGE data, int max_lines)
{
	return CALLJPEG(sp, -1, (int) jpeg_read_raw_data(&sp->cinfo.d,
	    data, (JDIMENSION) max_lines));
}