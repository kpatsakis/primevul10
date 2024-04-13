TIFFjpeg_finish_compress(JPEGState* sp)
{
	return CALLVJPEG(sp, jpeg_finish_compress(&sp->cinfo.c));
}