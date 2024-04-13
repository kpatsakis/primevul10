TIFFjpeg_abort(JPEGState* sp)
{
	return CALLVJPEG(sp, jpeg_abort(&sp->cinfo.comm));
}