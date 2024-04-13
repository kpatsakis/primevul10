JPEGPostEncode(TIFF* tif)
{
	JPEGState *sp = JState(tif);

	if (sp->scancount > 0) {
		/*
		 * Need to emit a partial bufferload of downsampled data.
		 * Pad the data vertically.
		 */
		int ci, ypos, n;
		jpeg_component_info* compptr;

		for (ci = 0, compptr = sp->cinfo.c.comp_info;
		     ci < sp->cinfo.c.num_components;
		     ci++, compptr++) {
			int vsamp = compptr->v_samp_factor;
			tmsize_t row_width = compptr->width_in_blocks * DCTSIZE
				* sizeof(JSAMPLE);
			for (ypos = sp->scancount * vsamp;
			     ypos < DCTSIZE * vsamp; ypos++) {
				_TIFFmemcpy((void*)sp->ds_buffer[ci][ypos],
					    (void*)sp->ds_buffer[ci][ypos-1],
					    row_width);

			}
		}
		n = sp->cinfo.c.max_v_samp_factor * DCTSIZE;
		if (TIFFjpeg_write_raw_data(sp, sp->ds_buffer, n) != n)
			return (0);
	}

	return (TIFFjpeg_finish_compress(JState(tif)));
}