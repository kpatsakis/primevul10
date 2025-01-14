TIFFFlushData1(TIFF* tif)
{
	if (tif->tif_rawcc > 0 && tif->tif_flags & TIFF_BUF4WRITE ) {
		if (!isFillOrder(tif, tif->tif_dir.td_fillorder) &&
		    (tif->tif_flags & TIFF_NOBITREV) == 0)
			TIFFReverseBits((uint8*)tif->tif_rawdata,
			    tif->tif_rawcc);
		if (!TIFFAppendToStrip(tif,
		    isTiled(tif) ? tif->tif_curtile : tif->tif_curstrip,
		    tif->tif_rawdata, tif->tif_rawcc))
        {
            /* We update those variables even in case of error since there's */
            /* code that doesn't really check the return code of this */
            /* function */
            tif->tif_rawcc = 0;
            tif->tif_rawcp = tif->tif_rawdata;
			return (0);
        }
		tif->tif_rawcc = 0;
		tif->tif_rawcp = tif->tif_rawdata;
	}
	return (1);
}