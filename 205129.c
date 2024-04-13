TIFFWriteEncodedStrip(TIFF* tif, uint32 strip, void* data, tmsize_t cc)
{
	static const char module[] = "TIFFWriteEncodedStrip";
	TIFFDirectory *td = &tif->tif_dir;
	uint16 sample;

	if (!WRITECHECKSTRIPS(tif, module))
		return ((tmsize_t) -1);
	/*
	 * Check strip array to make sure there's space.
	 * We don't support dynamically growing files that
	 * have data organized in separate bitplanes because
	 * it's too painful.  In that case we require that
	 * the imagelength be set properly before the first
	 * write (so that the strips array will be fully
	 * allocated above).
	 */
	if (strip >= td->td_nstrips) {
		if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
			TIFFErrorExt(tif->tif_clientdata, module,
			    "Can not grow image by strips when using separate planes");
			return ((tmsize_t) -1);
		}
		if (!TIFFGrowStrips(tif, 1, module))
			return ((tmsize_t) -1);
		td->td_stripsperimage =
		    TIFFhowmany_32(td->td_imagelength, td->td_rowsperstrip);  
	}
	/*
	 * Handle delayed allocation of data buffer.  This
	 * permits it to be sized according to the directory
	 * info.
	 */
	if (!BUFFERCHECK(tif))
		return ((tmsize_t) -1);

        tif->tif_flags |= TIFF_BUF4WRITE;
	tif->tif_curstrip = strip;

        if (td->td_stripsperimage == 0) {
                TIFFErrorExt(tif->tif_clientdata, module, "Zero strips per image");
                return ((tmsize_t) -1);
        }

	tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
	if ((tif->tif_flags & TIFF_CODERSETUP) == 0) {
		if (!(*tif->tif_setupencode)(tif))
			return ((tmsize_t) -1);
		tif->tif_flags |= TIFF_CODERSETUP;
	}

	if( td->td_stripbytecount[strip] > 0 )
        {
            /* Make sure that at the first attempt of rewriting the tile, we will have */
            /* more bytes available in the output buffer than the previous byte count, */
            /* so that TIFFAppendToStrip() will detect the overflow when it is called the first */
            /* time if the new compressed tile is bigger than the older one. (GDAL #4771) */
            if( tif->tif_rawdatasize <= (tmsize_t)td->td_stripbytecount[strip] )
            {
                if( !(TIFFWriteBufferSetup(tif, NULL,
                    (tmsize_t)TIFFroundup_64((uint64)(td->td_stripbytecount[strip] + 1), 1024))) )
                    return ((tmsize_t)(-1));
            }

	    /* Force TIFFAppendToStrip() to consider placing data at end
               of file. */
            tif->tif_curoff = 0;
        }

    tif->tif_rawcc = 0;
    tif->tif_rawcp = tif->tif_rawdata;

	tif->tif_flags &= ~TIFF_POSTENCODE;

    /* shortcut to avoid an extra memcpy() */
    if( td->td_compression == COMPRESSION_NONE )
    {
        /* swab if needed - note that source buffer will be altered */
        tif->tif_postdecode( tif, (uint8*) data, cc );

        if (!isFillOrder(tif, td->td_fillorder) &&
            (tif->tif_flags & TIFF_NOBITREV) == 0)
            TIFFReverseBits((uint8*) data, cc);

        if (cc > 0 &&
            !TIFFAppendToStrip(tif, strip, (uint8*) data, cc))
            return ((tmsize_t) -1);
        return (cc);
    }

	sample = (uint16)(strip / td->td_stripsperimage);
	if (!(*tif->tif_preencode)(tif, sample))
		return ((tmsize_t) -1);

        /* swab if needed - note that source buffer will be altered */
	tif->tif_postdecode( tif, (uint8*) data, cc );

	if (!(*tif->tif_encodestrip)(tif, (uint8*) data, cc, sample))
		return ((tmsize_t) -1);
	if (!(*tif->tif_postencode)(tif))
		return ((tmsize_t) -1);
	if (!isFillOrder(tif, td->td_fillorder) &&
	    (tif->tif_flags & TIFF_NOBITREV) == 0)
		TIFFReverseBits(tif->tif_rawdata, tif->tif_rawcc);
	if (tif->tif_rawcc > 0 &&
	    !TIFFAppendToStrip(tif, strip, tif->tif_rawdata, tif->tif_rawcc))
		return ((tmsize_t) -1);
	tif->tif_rawcc = 0;
	tif->tif_rawcp = tif->tif_rawdata;
	return (cc);
}