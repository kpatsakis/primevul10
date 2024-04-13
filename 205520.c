JPEGSetupDecode(TIFF* tif)
{
	JPEGState* sp = JState(tif);
	TIFFDirectory *td = &tif->tif_dir;

#if defined(JPEG_DUAL_MODE_8_12) && !defined(TIFFInitJPEG)
        if( tif->tif_dir.td_bitspersample == 12 )
            return TIFFReInitJPEG_12( tif, COMPRESSION_JPEG, 0 );
#endif

	JPEGInitializeLibJPEG( tif, TRUE );

	assert(sp != NULL);
	assert(sp->cinfo.comm.is_decompressor);

	/* Read JPEGTables if it is present */
	if (TIFFFieldSet(tif,FIELD_JPEGTABLES)) {
		TIFFjpeg_tables_src(sp, tif);
		if(TIFFjpeg_read_header(sp,FALSE) != JPEG_HEADER_TABLES_ONLY) {
			TIFFErrorExt(tif->tif_clientdata, "JPEGSetupDecode", "Bogus JPEGTables field");
			return (0);
		}
	}

	/* Grab parameters that are same for all strips/tiles */
	sp->photometric = td->td_photometric;
	switch (sp->photometric) {
	case PHOTOMETRIC_YCBCR:
		sp->h_sampling = td->td_ycbcrsubsampling[0];
		sp->v_sampling = td->td_ycbcrsubsampling[1];
		break;
	default:
		/* TIFF 6.0 forbids subsampling of all other color spaces */
		sp->h_sampling = 1;
		sp->v_sampling = 1;
		break;
	}

	/* Set up for reading normal data */
	TIFFjpeg_data_src(sp, tif);
	tif->tif_postdecode = _TIFFNoPostDecode; /* override byte swapping */
	return (1);
}