JPEGResetUpsampled( TIFF* tif )
{
	JPEGState* sp = JState(tif);
	TIFFDirectory* td = &tif->tif_dir;

	/*
	 * Mark whether returned data is up-sampled or not so TIFFStripSize
	 * and TIFFTileSize return values that reflect the true amount of
	 * data.
	 */
	tif->tif_flags &= ~TIFF_UPSAMPLED;
	if (td->td_planarconfig == PLANARCONFIG_CONTIG) {
		if (td->td_photometric == PHOTOMETRIC_YCBCR &&
		    sp->jpegcolormode == JPEGCOLORMODE_RGB) {
			tif->tif_flags |= TIFF_UPSAMPLED;
		} else {
#ifdef notdef
			if (td->td_ycbcrsubsampling[0] != 1 ||
			    td->td_ycbcrsubsampling[1] != 1)
				; /* XXX what about up-sampling? */
#endif
		}
	}

	/*
	 * Must recalculate cached tile size in case sampling state changed.
	 * Should we really be doing this now if image size isn't set? 
	 */
        if( tif->tif_tilesize > 0 )
            tif->tif_tilesize = isTiled(tif) ? TIFFTileSize(tif) : (tmsize_t)(-1);   
        if( tif->tif_scanlinesize > 0 )
            tif->tif_scanlinesize = TIFFScanlineSize(tif); 
}