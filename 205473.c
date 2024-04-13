TIFFjpeg_finish_decompress(JPEGState* sp)
{
	return CALLJPEG(sp, -1, (int) jpeg_finish_decompress(&sp->cinfo.d));
}