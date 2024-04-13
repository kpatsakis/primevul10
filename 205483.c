TIFFjpeg_data_dest(JPEGState* sp, TIFF* tif)
{
	(void) tif;
	sp->cinfo.c.dest = &sp->dest;
	sp->dest.init_destination = std_init_destination;
	sp->dest.empty_output_buffer = std_empty_output_buffer;
	sp->dest.term_destination = std_term_destination;
}