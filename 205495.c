TIFFjpeg_read_header(JPEGState* sp, boolean require_image)
{
	return CALLJPEG(sp, -1, jpeg_read_header(&sp->cinfo.d, require_image));
}