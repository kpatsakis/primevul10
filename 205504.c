JPEGCleanup(TIFF* tif)
{
	JPEGState *sp = JState(tif);
	
	assert(sp != 0);

	tif->tif_tagmethods.vgetfield = sp->vgetparent;
	tif->tif_tagmethods.vsetfield = sp->vsetparent;
	tif->tif_tagmethods.printdir = sp->printdir;
        if( sp->cinfo_initialized )
                TIFFjpeg_destroy(sp);	/* release libjpeg resources */
        if (sp->jpegtables)		/* tag value */
                _TIFFfree(sp->jpegtables);
	_TIFFfree(tif->tif_data);	/* release local state */
	tif->tif_data = NULL;

	_TIFFSetDefaultCompressionState(tif);
}