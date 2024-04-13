JPEGPreEncode(TIFF* tif, uint16 s)
{
	JPEGState *sp = JState(tif);
	TIFFDirectory *td = &tif->tif_dir;
	static const char module[] = "JPEGPreEncode";
	uint32 segment_width, segment_height;
	int downsampled_input;

	assert(sp != NULL);
  
	if (sp->cinfo.comm.is_decompressor == 1)
	{
		tif->tif_setupencode( tif );
	}
  
	assert(!sp->cinfo.comm.is_decompressor);
	/*
	 * Set encoding parameters for this strip/tile.
	 */
	if (isTiled(tif)) {
		segment_width = td->td_tilewidth;
		segment_height = td->td_tilelength;
		sp->bytesperline = TIFFTileRowSize(tif);
	} else {
		segment_width = td->td_imagewidth;
		segment_height = td->td_imagelength - tif->tif_row;
		if (segment_height > td->td_rowsperstrip)
			segment_height = td->td_rowsperstrip;
		sp->bytesperline = TIFFScanlineSize(tif);
	}
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE && s > 0) {
		/* for PC 2, scale down the strip/tile size
		 * to match a downsampled component
		 */
		segment_width = TIFFhowmany_32(segment_width, sp->h_sampling); 
		segment_height = TIFFhowmany_32(segment_height, sp->v_sampling);
	}
	if (segment_width > 65535 || segment_height > 65535) {
		TIFFErrorExt(tif->tif_clientdata, module, "Strip/tile too large for JPEG");
		return (0);
	}
	sp->cinfo.c.image_width = segment_width;
	sp->cinfo.c.image_height = segment_height;
	downsampled_input = FALSE;
	if (td->td_planarconfig == PLANARCONFIG_CONTIG) {
		sp->cinfo.c.input_components = td->td_samplesperpixel;
		if (sp->photometric == PHOTOMETRIC_YCBCR) {
			if (sp->jpegcolormode != JPEGCOLORMODE_RGB) {
				if (sp->h_sampling != 1 || sp->v_sampling != 1)
					downsampled_input = TRUE;
			}
			if (!TIFFjpeg_set_colorspace(sp, JCS_YCbCr))
				return (0);
			/*
			 * Set Y sampling factors;
			 * we assume jpeg_set_colorspace() set the rest to 1
			 */
			sp->cinfo.c.comp_info[0].h_samp_factor = sp->h_sampling;
			sp->cinfo.c.comp_info[0].v_samp_factor = sp->v_sampling;
		} else {
			if (!TIFFjpeg_set_colorspace(sp, sp->cinfo.c.in_color_space))
				return (0);
			/* jpeg_set_colorspace set all sampling factors to 1 */
		}
	} else {
		if (!TIFFjpeg_set_colorspace(sp, JCS_UNKNOWN))
			return (0);
		sp->cinfo.c.comp_info[0].component_id = s;
		/* jpeg_set_colorspace() set sampling factors to 1 */
		if (sp->photometric == PHOTOMETRIC_YCBCR && s > 0) {
			sp->cinfo.c.comp_info[0].quant_tbl_no = 1;
			sp->cinfo.c.comp_info[0].dc_tbl_no = 1;
			sp->cinfo.c.comp_info[0].ac_tbl_no = 1;
		}
	}
	/* ensure libjpeg won't write any extraneous markers */
	sp->cinfo.c.write_JFIF_header = FALSE;
	sp->cinfo.c.write_Adobe_marker = FALSE;
	/* set up table handling correctly */
	/* calling TIFFjpeg_set_quality() causes quantization tables to be flagged */
	/* as being to be emitted, which we don't want in the JPEGTABLESMODE_QUANT */
	/* mode, so we must manually suppress them. However TIFFjpeg_set_quality() */
	/* should really be called when dealing with files with directories with */
	/* mixed qualities. see http://trac.osgeo.org/gdal/ticket/3539 */
	if (!TIFFjpeg_set_quality(sp, sp->jpegquality, FALSE))
		return (0);
	if (sp->jpegtablesmode & JPEGTABLESMODE_QUANT) {
		suppress_quant_table(sp, 0);
		suppress_quant_table(sp, 1);
	}
	else {
		unsuppress_quant_table(sp, 0);
		unsuppress_quant_table(sp, 1);
	}
	if (sp->jpegtablesmode & JPEGTABLESMODE_HUFF)
	{
		/* Explicit suppression is only needed if we did not go through the */
		/* prepare_JPEGTables() code path, which may be the case if updating */
		/* an existing file */
		suppress_huff_table(sp, 0);
		suppress_huff_table(sp, 1);
		sp->cinfo.c.optimize_coding = FALSE;
	}
	else
		sp->cinfo.c.optimize_coding = TRUE;
	if (downsampled_input) {
		/* Need to use raw-data interface to libjpeg */
		sp->cinfo.c.raw_data_in = TRUE;
		tif->tif_encoderow = JPEGEncodeRaw;
		tif->tif_encodestrip = JPEGEncodeRaw;
		tif->tif_encodetile = JPEGEncodeRaw;
	} else {
		/* Use normal interface to libjpeg */
		sp->cinfo.c.raw_data_in = FALSE;
		tif->tif_encoderow = JPEGEncode;
		tif->tif_encodestrip = JPEGEncode;
		tif->tif_encodetile = JPEGEncode;
	}
	/* Start JPEG compressor */
	if (!TIFFjpeg_start_compress(sp, FALSE))
		return (0);
	/* Allocate downsampled-data buffers if needed */
	if (downsampled_input) {
		if (!alloc_downsampled_buffers(tif, sp->cinfo.c.comp_info,
					       sp->cinfo.c.num_components))
			return (0);
	}
	sp->scancount = 0;

	return (1);
}