std_init_source(j_decompress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;
	TIFF* tif = sp->tif;

	sp->src.next_input_byte = (const JOCTET*) tif->tif_rawdata;
	sp->src.bytes_in_buffer = (size_t) tif->tif_rawcc;
}