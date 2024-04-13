JPEGEncode(TIFF* tif, uint8* buf, tmsize_t cc, uint16 s)
{
	JPEGState *sp = JState(tif);
	tmsize_t nrows;
	JSAMPROW bufptr[1];
        short *line16 = NULL;
        int    line16_count = 0;

	(void) s;
	assert(sp != NULL);
	/* data is expected to be supplied in multiples of a scanline */
	nrows = cc / sp->bytesperline;
	if (cc % sp->bytesperline)
            TIFFWarningExt(tif->tif_clientdata, tif->tif_name, 
                           "fractional scanline discarded");

        /* The last strip will be limited to image size */
        if( !isTiled(tif) && tif->tif_row+nrows > tif->tif_dir.td_imagelength )
            nrows = tif->tif_dir.td_imagelength - tif->tif_row;

        if( sp->cinfo.c.data_precision == 12 )
        {
            line16_count = (int)((sp->bytesperline * 2) / 3);
            line16 = (short *) _TIFFmalloc(sizeof(short) * line16_count);
            if (!line16)
            {
                TIFFErrorExt(tif->tif_clientdata,
			     "JPEGEncode",
                             "Failed to allocate memory");

                return 0;
            }
        }
            
	while (nrows-- > 0) {

            if( sp->cinfo.c.data_precision == 12 )
            {

                int value_pairs = line16_count / 2;
                int iPair;

		bufptr[0] = (JSAMPROW) line16;

                for( iPair = 0; iPair < value_pairs; iPair++ )
                {
                    unsigned char *in_ptr =
                        ((unsigned char *) buf) + iPair * 3;
                    JSAMPLE *out_ptr = (JSAMPLE *) (line16 + iPair * 2);

                    out_ptr[0] = (in_ptr[0] << 4) | ((in_ptr[1] & 0xf0) >> 4);
                    out_ptr[1] = ((in_ptr[1] & 0x0f) << 8) | in_ptr[2];
                }
            }
            else
            {
		bufptr[0] = (JSAMPROW) buf;
            }
            if (TIFFjpeg_write_scanlines(sp, bufptr, 1) != 1)
                return (0);
            if (nrows > 0)
                tif->tif_row++;
            buf += sp->bytesperline;
	}

        if( sp->cinfo.c.data_precision == 12 )
        {
            _TIFFfree( line16 );
        }
            
	return (1);
}