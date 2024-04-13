prepare_JPEGTables(TIFF* tif)
{
	JPEGState* sp = JState(tif);

	/* Initialize quant tables for current quality setting */
	if (!TIFFjpeg_set_quality(sp, sp->jpegquality, FALSE))
		return (0);
	/* Mark only the tables we want for output */
	/* NB: chrominance tables are currently used only with YCbCr */
	if (!TIFFjpeg_suppress_tables(sp, TRUE))
		return (0);
	if (sp->jpegtablesmode & JPEGTABLESMODE_QUANT) {
		unsuppress_quant_table(sp, 0);
		if (sp->photometric == PHOTOMETRIC_YCBCR)
			unsuppress_quant_table(sp, 1);
	}
	if (sp->jpegtablesmode & JPEGTABLESMODE_HUFF) {
		unsuppress_huff_table(sp, 0);
		if (sp->photometric == PHOTOMETRIC_YCBCR)
			unsuppress_huff_table(sp, 1);
	}
	/* Direct libjpeg output into jpegtables */
	if (!TIFFjpeg_tables_dest(sp, tif))
		return (0);
	/* Emit tables-only datastream */
	if (!TIFFjpeg_write_tables(sp))
		return (0);

	return (1);
}