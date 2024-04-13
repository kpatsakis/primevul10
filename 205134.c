TIFFWriteRawStrip(TIFF* tif, uint32 strip, void* data, tmsize_t cc)
{
	static const char module[] = "TIFFWriteRawStrip";
	TIFFDirectory *td = &tif->tif_dir;

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
		/*
		 * Watch out for a growing image.  The value of
		 * strips/image will initially be 1 (since it
		 * can't be deduced until the imagelength is known).
		 */
		if (strip >= td->td_stripsperimage)
			td->td_stripsperimage =
			    TIFFhowmany_32(td->td_imagelength,td->td_rowsperstrip);
		if (!TIFFGrowStrips(tif, 1, module))
			return ((tmsize_t) -1);
	}
	tif->tif_curstrip = strip;
        if (td->td_stripsperimage == 0) {
                TIFFErrorExt(tif->tif_clientdata, module,"Zero strips per image");
                return ((tmsize_t) -1);
        }
	tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
	return (TIFFAppendToStrip(tif, strip, (uint8*) data, cc) ?
	    cc : (tmsize_t) -1);
}