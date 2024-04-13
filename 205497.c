TIFFInitJPEG(TIFF* tif, int scheme)
{
	JPEGState* sp;

	assert(scheme == COMPRESSION_JPEG);

	/*
	 * Merge codec-specific tag information.
	 */
	if (!_TIFFMergeFields(tif, jpegFields, TIFFArrayCount(jpegFields))) {
		TIFFErrorExt(tif->tif_clientdata,
			     "TIFFInitJPEG",
			     "Merging JPEG codec-specific tags failed");
		return 0;
	}

	/*
	 * Allocate state block so tag methods have storage to record values.
	 */
	tif->tif_data = (uint8*) _TIFFmalloc(sizeof (JPEGState));

	if (tif->tif_data == NULL) {
		TIFFErrorExt(tif->tif_clientdata,
			     "TIFFInitJPEG", "No space for JPEG state block");
		return 0;
	}
        _TIFFmemset(tif->tif_data, 0, sizeof(JPEGState));

	sp = JState(tif);
	sp->tif = tif;				/* back link */

	/*
	 * Override parent get/set field methods.
	 */
	sp->vgetparent = tif->tif_tagmethods.vgetfield;
	tif->tif_tagmethods.vgetfield = JPEGVGetField; /* hook for codec tags */
	sp->vsetparent = tif->tif_tagmethods.vsetfield;
	tif->tif_tagmethods.vsetfield = JPEGVSetField; /* hook for codec tags */
	sp->printdir = tif->tif_tagmethods.printdir;
	tif->tif_tagmethods.printdir = JPEGPrintDir;   /* hook for codec tags */

	/* Default values for codec-specific fields */
	sp->jpegtables = NULL;
	sp->jpegtables_length = 0;
	sp->jpegquality = 75;			/* Default IJG quality */
	sp->jpegcolormode = JPEGCOLORMODE_RAW;
	sp->jpegtablesmode = JPEGTABLESMODE_QUANT | JPEGTABLESMODE_HUFF;
        sp->ycbcrsampling_fetched = 0;

	/*
	 * Install codec methods.
	 */
	tif->tif_fixuptags = JPEGFixupTags;
	tif->tif_setupdecode = JPEGSetupDecode;
	tif->tif_predecode = JPEGPreDecode;
	tif->tif_decoderow = JPEGDecode;
	tif->tif_decodestrip = JPEGDecode;
	tif->tif_decodetile = JPEGDecode;
	tif->tif_setupencode = JPEGSetupEncode;
	tif->tif_preencode = JPEGPreEncode;
	tif->tif_postencode = JPEGPostEncode;
	tif->tif_encoderow = JPEGEncode;
	tif->tif_encodestrip = JPEGEncode;
	tif->tif_encodetile = JPEGEncode;  
	tif->tif_cleanup = JPEGCleanup;
	sp->defsparent = tif->tif_defstripsize;
	tif->tif_defstripsize = JPEGDefaultStripSize;
	sp->deftparent = tif->tif_deftilesize;
	tif->tif_deftilesize = JPEGDefaultTileSize;
	tif->tif_flags |= TIFF_NOBITREV;	/* no bit reversal, please */

        sp->cinfo_initialized = FALSE;

	/*
        ** Create a JPEGTables field if no directory has yet been created. 
        ** We do this just to ensure that sufficient space is reserved for
        ** the JPEGTables field.  It will be properly created the right
        ** size later. 
        */
        if( tif->tif_diroff == 0 )
        {
#define SIZE_OF_JPEGTABLES 2000
/*
The following line assumes incorrectly that all JPEG-in-TIFF files will have
a JPEGTABLES tag generated and causes null-filled JPEGTABLES tags to be written
when the JPEG data is placed with TIFFWriteRawStrip.  The field bit should be 
set, anyway, later when actual JPEGTABLES header is generated, so removing it 
here hopefully is harmless.
            TIFFSetFieldBit(tif, FIELD_JPEGTABLES);
*/
            sp->jpegtables_length = SIZE_OF_JPEGTABLES;
            sp->jpegtables = (void *) _TIFFmalloc(sp->jpegtables_length);
            if (sp->jpegtables)
            {
                _TIFFmemset(sp->jpegtables, 0, SIZE_OF_JPEGTABLES);
            }
            else
            {
                TIFFErrorExt(tif->tif_clientdata,
			     "TIFFInitJPEG",
                             "Failed to allocate memory for JPEG tables");
                return 0;
            }
#undef SIZE_OF_JPEGTABLES
        }

	return 1;
}