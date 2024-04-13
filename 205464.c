TIFFjpeg_start_decompress(JPEGState* sp)
{
	return CALLVJPEG(sp, jpeg_start_decompress(&sp->cinfo.d));
}