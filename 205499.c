std_term_destination(j_compress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;
	TIFF* tif = sp->tif;

	tif->tif_rawcp = (uint8*) sp->dest.next_output_byte;
	tif->tif_rawcc =
	    tif->tif_rawdatasize - (tmsize_t) sp->dest.free_in_buffer;
	/* NB: libtiff does the final buffer flush */
}