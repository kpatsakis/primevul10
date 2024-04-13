TIFFjpeg_create_compress(JPEGState* sp)
{
	/* initialize JPEG error handling */
	sp->cinfo.c.err = jpeg_std_error(&sp->err);
	sp->err.error_exit = TIFFjpeg_error_exit;
	sp->err.output_message = TIFFjpeg_output_message;

	/* set client_data to avoid UMR warning from tools like Purify */
	sp->cinfo.c.client_data = NULL;

	return CALLVJPEG(sp, jpeg_create_compress(&sp->cinfo.c));
}