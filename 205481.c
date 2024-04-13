JPEGDefaultStripSize(TIFF* tif, uint32 s)
{
	JPEGState* sp = JState(tif);
	TIFFDirectory *td = &tif->tif_dir;

	s = (*sp->defsparent)(tif, s);
	if (s < td->td_imagelength)
		s = TIFFroundup_32(s, td->td_ycbcrsubsampling[1] * DCTSIZE);
	return (s);
}