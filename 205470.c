JPEGDefaultTileSize(TIFF* tif, uint32* tw, uint32* th)
{
	JPEGState* sp = JState(tif);
	TIFFDirectory *td = &tif->tif_dir;

	(*sp->deftparent)(tif, tw, th);
	*tw = TIFFroundup_32(*tw, td->td_ycbcrsubsampling[0] * DCTSIZE);
	*th = TIFFroundup_32(*th, td->td_ycbcrsubsampling[1] * DCTSIZE);
}