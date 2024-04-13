JPEGDecode(TIFF* tif, uint8* buf, tmsize_t cc, uint16 s)
{
	JPEGState *sp = JState(tif);
	tmsize_t nrows;
	(void) s;

        /*
        ** Update available information, buffer may have been refilled
        ** between decode requests
        */
	sp->src.next_input_byte = (const JOCTET*) tif->tif_rawcp;
	sp->src.bytes_in_buffer = (size_t) tif->tif_rawcc;

        if( sp->bytesperline == 0 )
                return 0;
        
	nrows = cc / sp->bytesperline;
	if (cc % sp->bytesperline)
		TIFFWarningExt(tif->tif_clientdata, tif->tif_name,
                               "fractional scanline not read");

	if( nrows > (tmsize_t) sp->cinfo.d.image_height )
		nrows = sp->cinfo.d.image_height;

	/* data is expected to be read in multiples of a scanline */
	if (nrows)
        {
                do
                {
                        /*
                         * In the libjpeg6b-9a 8bit case.  We read directly into
                         * the TIFF buffer.
                         */
                        JSAMPROW bufptr = (JSAMPROW)buf;

                        if (TIFFjpeg_read_scanlines(sp, &bufptr, 1) != 1)
                                return (0);

                        ++tif->tif_row;
                        buf += sp->bytesperline;
                        cc -= sp->bytesperline;
                } while (--nrows > 0);
        }

        /* Update information on consumed data */
        tif->tif_rawcp = (uint8*) sp->src.next_input_byte;
        tif->tif_rawcc = sp->src.bytes_in_buffer;
                
	/* Close down the decompressor if we've finished the strip or tile. */
	return sp->cinfo.d.output_scanline < sp->cinfo.d.output_height
                || TIFFjpeg_finish_decompress(sp);
}