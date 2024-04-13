TIFFjpeg_data_src(JPEGState* sp, TIFF* tif)
{
	(void) tif;
	sp->cinfo.d.src = &sp->src;
	sp->src.init_source = std_init_source;
	sp->src.fill_input_buffer = std_fill_input_buffer;
	sp->src.skip_input_data = std_skip_input_data;
	sp->src.resync_to_restart = jpeg_resync_to_restart;
	sp->src.term_source = std_term_source;
	sp->src.bytes_in_buffer = 0;		/* for safety */
	sp->src.next_input_byte = NULL;
}