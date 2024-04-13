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
                JSAMPROW line_work_buf = NULL;

                /*
                 * For 6B, only use temporary buffer for 12 bit imagery.
                 * For Mk1 always use it.
                 */
                if( sp->cinfo.d.data_precision == 12 )
                {
                        line_work_buf = (JSAMPROW)
                                _TIFFmalloc(sizeof(short) * sp->cinfo.d.output_width
                                            * sp->cinfo.d.num_components );
                }

               do
               {
                       if( line_work_buf != NULL )
                       {
                               /*
                                * In the MK1 case, we always read into a 16bit
                                * buffer, and then pack down to 12bit or 8bit.
                                * In 6B case we only read into 16 bit buffer
                                * for 12bit data, which we need to repack.
                                */
                               if (TIFFjpeg_read_scanlines(sp, &line_work_buf, 1) != 1)
                                       return (0);

                               if( sp->cinfo.d.data_precision == 12 )
                               {
                                       int value_pairs = (sp->cinfo.d.output_width
                                                          * sp->cinfo.d.num_components) / 2;
                                       int iPair;

                                       for( iPair = 0; iPair < value_pairs; iPair++ )
                                       {
                                               unsigned char *out_ptr =
                                                       ((unsigned char *) buf) + iPair * 3;
                                               JSAMPLE *in_ptr = line_work_buf + iPair * 2;

                                               out_ptr[0] = (unsigned char)((in_ptr[0] & 0xff0) >> 4);
                                               out_ptr[1] = (unsigned char)(((in_ptr[0] & 0xf) << 4)
                                                       | ((in_ptr[1] & 0xf00) >> 8));
                                               out_ptr[2] = (unsigned char)(((in_ptr[1] & 0xff) >> 0));
                                       }
                               }
                               else if( sp->cinfo.d.data_precision == 8 )
                               {
                                       int value_count = (sp->cinfo.d.output_width
                                                          * sp->cinfo.d.num_components);
                                       int iValue;

                                       for( iValue = 0; iValue < value_count; iValue++ )
                                       {
                                               ((unsigned char *) buf)[iValue] =
                                                       line_work_buf[iValue] & 0xff;
                                       }
                               }
                       }

                       ++tif->tif_row;
                       buf += sp->bytesperline;
                       cc -= sp->bytesperline;
               } while (--nrows > 0);

               if( line_work_buf != NULL )
                       _TIFFfree( line_work_buf );
        }

        /* Update information on consumed data */
        tif->tif_rawcp = (uint8*) sp->src.next_input_byte;
        tif->tif_rawcc = sp->src.bytes_in_buffer;
                
	/* Close down the decompressor if we've finished the strip or tile. */
	return sp->cinfo.d.output_scanline < sp->cinfo.d.output_height
                || TIFFjpeg_finish_decompress(sp);
}