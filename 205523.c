JPEGFixupTagsSubsampling(TIFF* tif)
{
	/*
	 * Some JPEG-in-TIFF produces do not emit the YCBCRSUBSAMPLING values in
	 * the TIFF tags, but still use non-default (2,2) values within the jpeg
	 * data stream itself.  In order for TIFF applications to work properly
	 * - for instance to get the strip buffer size right - it is imperative
	 * that the subsampling be available before we start reading the image
	 * data normally.  This function will attempt to analyze the first strip in
	 * order to get the sampling values from the jpeg data stream.
	 *
	 * Note that JPEGPreDeocode() will produce a fairly loud warning when the
	 * discovered sampling does not match the default sampling (2,2) or whatever
	 * was actually in the tiff tags.
	 *
	 * See the bug in bugzilla for details:
	 *
	 * http://bugzilla.remotesensing.org/show_bug.cgi?id=168
	 *
	 * Frank Warmerdam, July 2002
	 * Joris Van Damme, May 2007
	 */
	static const char module[] = "JPEGFixupTagsSubsampling";
	struct JPEGFixupTagsSubsamplingData m;

        _TIFFFillStriles( tif );
        
        if( tif->tif_dir.td_stripbytecount == NULL
            || tif->tif_dir.td_stripoffset == NULL
            || tif->tif_dir.td_stripbytecount[0] == 0 )
        {
            /* Do not even try to check if the first strip/tile does not
               yet exist, as occurs when GDAL has created a new NULL file
               for instance. */
            return;
        }

	m.tif=tif;
	m.buffersize=2048;
	m.buffer=_TIFFmalloc(m.buffersize);
	if (m.buffer==NULL)
	{
		TIFFWarningExt(tif->tif_clientdata,module,
		    "Unable to allocate memory for auto-correcting of subsampling values; auto-correcting skipped");
		return;
	}
	m.buffercurrentbyte=NULL;
	m.bufferbytesleft=0;
	m.fileoffset=tif->tif_dir.td_stripoffset[0];
	m.filepositioned=0;
	m.filebytesleft=tif->tif_dir.td_stripbytecount[0];
	if (!JPEGFixupTagsSubsamplingSec(&m))
		TIFFWarningExt(tif->tif_clientdata,module,
		    "Unable to auto-correct subsampling values, likely corrupt JPEG compressed data in first strip/tile; auto-correcting skipped");
	_TIFFfree(m.buffer);
}