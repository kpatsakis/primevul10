std_fill_input_buffer(j_decompress_ptr cinfo)
{
	JPEGState* sp = (JPEGState* ) cinfo;
	static const JOCTET dummy_EOI[2] = { 0xFF, JPEG_EOI };

#ifdef IPPJ_HUFF
        /*
         * The Intel IPP performance library does not necessarily read the whole
         * input buffer in one pass, so it is possible to get here with data
         * yet to read. 
         * 
         * We just return without doing anything, until the entire buffer has
         * been read.  
         * http://trac.osgeo.org/gdal/wiki/JpegIPP
         */
        if( sp->src.bytes_in_buffer > 0 ) {
            return (TRUE);
        }
#endif

	/*
         * Normally the whole strip/tile is read and so we don't need to do
         * a fill.  In the case of CHUNKY_STRIP_READ_SUPPORT we might not have
         * all the data, but the rawdata is refreshed between scanlines and
         * we push this into the io machinery in JPEGDecode(). 	 
         * http://trac.osgeo.org/gdal/ticket/3894
	 */
        
	WARNMS(cinfo, JWRN_JPEG_EOF);
	/* insert a fake EOI marker */
	sp->src.next_input_byte = dummy_EOI;
	sp->src.bytes_in_buffer = 2;
	return (TRUE);
}