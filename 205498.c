JPEGDecodeRaw(TIFF* tif, uint8* buf, tmsize_t cc, uint16 s)
{
	JPEGState *sp = JState(tif);
	tmsize_t nrows;
	(void) s;

	/* data is expected to be read in multiples of a scanline */
	if ( (nrows = sp->cinfo.d.image_height) != 0 ) {

		/* Cb,Cr both have sampling factors 1, so this is correct */
		JDIMENSION clumps_per_line = sp->cinfo.d.comp_info[1].downsampled_width;            
		int samples_per_clump = sp->samplesperclump;

#if defined(JPEG_LIB_MK1_OR_12BIT)
		unsigned short* tmpbuf = _TIFFmalloc(sizeof(unsigned short) *
						     sp->cinfo.d.output_width *
						     sp->cinfo.d.num_components);
		if(tmpbuf==NULL) {
                        TIFFErrorExt(tif->tif_clientdata, "JPEGDecodeRaw",
				     "Out of memory");
			return 0;
                }
#endif

		do {
			jpeg_component_info *compptr;
			int ci, clumpoffset;

                        if( cc < sp->bytesperline ) {
				TIFFErrorExt(tif->tif_clientdata, "JPEGDecodeRaw",
					     "application buffer not large enough for all data.");
				return 0;
                        }

			/* Reload downsampled-data buffer if needed */
			if (sp->scancount >= DCTSIZE) {
				int n = sp->cinfo.d.max_v_samp_factor * DCTSIZE;
				if (TIFFjpeg_read_raw_data(sp, sp->ds_buffer, n) != n)
					return (0);
				sp->scancount = 0;
			}
			/*
			 * Fastest way to unseparate data is to make one pass
			 * over the scanline for each row of each component.
			 */
			clumpoffset = 0;    /* first sample in clump */
			for (ci = 0, compptr = sp->cinfo.d.comp_info;
			     ci < sp->cinfo.d.num_components;
			     ci++, compptr++) {
				int hsamp = compptr->h_samp_factor;
				int vsamp = compptr->v_samp_factor;
				int ypos;

				for (ypos = 0; ypos < vsamp; ypos++) {
					JSAMPLE *inptr = sp->ds_buffer[ci][sp->scancount*vsamp + ypos];
					JDIMENSION nclump;
#if defined(JPEG_LIB_MK1_OR_12BIT)
					JSAMPLE *outptr = (JSAMPLE*)tmpbuf + clumpoffset;
#else
					JSAMPLE *outptr = (JSAMPLE*)buf + clumpoffset;
					if (cc < (tmsize_t) (clumpoffset + samples_per_clump*(clumps_per_line-1) + hsamp)) {
						TIFFErrorExt(tif->tif_clientdata, "JPEGDecodeRaw",
							     "application buffer not large enough for all data, possible subsampling issue");
						return 0;
					}
#endif

					if (hsamp == 1) {
						/* fast path for at least Cb and Cr */
						for (nclump = clumps_per_line; nclump-- > 0; ) {
							outptr[0] = *inptr++;
							outptr += samples_per_clump;
						}
					} else {
						int xpos;

						/* general case */
						for (nclump = clumps_per_line; nclump-- > 0; ) {
							for (xpos = 0; xpos < hsamp; xpos++)
								outptr[xpos] = *inptr++;
							outptr += samples_per_clump;
						}
					}
					clumpoffset += hsamp;
				}
			}

#if defined(JPEG_LIB_MK1_OR_12BIT)
			{
				if (sp->cinfo.d.data_precision == 8)
				{
					int i=0;
					int len = sp->cinfo.d.output_width * sp->cinfo.d.num_components;
					for (i=0; i<len; i++)
					{
						((unsigned char*)buf)[i] = tmpbuf[i] & 0xff;
					}
				}
				else
				{         /* 12-bit */
					int value_pairs = (sp->cinfo.d.output_width
							   * sp->cinfo.d.num_components) / 2;
					int iPair;
					for( iPair = 0; iPair < value_pairs; iPair++ )
					{
						unsigned char *out_ptr = ((unsigned char *) buf) + iPair * 3;
						JSAMPLE *in_ptr = (JSAMPLE *) (tmpbuf + iPair * 2);
						out_ptr[0] = (unsigned char)((in_ptr[0] & 0xff0) >> 4);
						out_ptr[1] = (unsigned char)(((in_ptr[0] & 0xf) << 4)
							| ((in_ptr[1] & 0xf00) >> 8));
						out_ptr[2] = (unsigned char)(((in_ptr[1] & 0xff) >> 0));
					}
				}
			}
#endif

			sp->scancount ++;
			tif->tif_row += sp->v_sampling;

			buf += sp->bytesperline;
			cc -= sp->bytesperline;

			nrows -= sp->v_sampling;
		} while (nrows > 0);

#if defined(JPEG_LIB_MK1_OR_12BIT)
		_TIFFfree(tmpbuf);
#endif

	}

	/* Close down the decompressor if done. */
	return sp->cinfo.d.output_scanline < sp->cinfo.d.output_height
		|| TIFFjpeg_finish_decompress(sp);
}