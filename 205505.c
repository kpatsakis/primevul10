std_init_destination(j_compress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;
	TIFF* tif = sp->tif;

	sp->dest.next_output_byte = (JOCTET*) tif->tif_rawdata;
	sp->dest.free_in_buffer = (size_t) tif->tif_rawdatasize;
}