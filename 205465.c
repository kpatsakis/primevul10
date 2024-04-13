TIFFjpeg_destroy(JPEGState* sp)
{
	return CALLVJPEG(sp, jpeg_destroy(&sp->cinfo.comm));
}