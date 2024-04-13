std_empty_output_buffer(j_compress_ptr cinfo)
{
	JPEGState* sp = (JPEGState*) cinfo;
	TIFF* tif = sp->tif;

	/* the entire buffer has been filled */
	tif->tif_rawcc = tif->tif_rawdatasize;

#ifdef IPPJ_HUFF
       /*
        * The Intel IPP performance library does not necessarily fill up
        * the whole output buffer on each pass, so only dump out the parts
        * that have been filled.
        *   http://trac.osgeo.org/gdal/wiki/JpegIPP
        */
       if ( sp->dest.free_in_buffer >= 0 ) {
               tif->tif_rawcc = tif->tif_rawdatasize - sp->dest.free_in_buffer;
       }
#endif

	TIFFFlushData1(tif);
	sp->dest.next_output_byte = (JOCTET*) tif->tif_rawdata;
	sp->dest.free_in_buffer = (size_t) tif->tif_rawdatasize;

	return (TRUE);
}