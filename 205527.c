TIFFjpeg_tables_dest(JPEGState* sp, TIFF* tif)
{
	(void) tif;
	/*
	 * Allocate a working buffer for building tables.
	 * Initial size is 1000 bytes, which is usually adequate.
	 */
	if (sp->jpegtables)
		_TIFFfree(sp->jpegtables);
	sp->jpegtables_length = 1000;
	sp->jpegtables = (void*) _TIFFmalloc((tmsize_t) sp->jpegtables_length);
	if (sp->jpegtables == NULL) {
		sp->jpegtables_length = 0;
		TIFFErrorExt(sp->tif->tif_clientdata, "TIFFjpeg_tables_dest", "No space for JPEGTables");
		return (0);
	}
	sp->cinfo.c.dest = &sp->dest;
	sp->dest.init_destination = tables_init_destination;
	sp->dest.empty_output_buffer = tables_empty_output_buffer;
	sp->dest.term_destination = tables_term_destination;
	return (1);
}