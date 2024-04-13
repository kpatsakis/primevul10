TIFFFdOpen(int ifd, const char* name, const char* mode)
{
	TIFF* tif;
	int fSuppressMap;
	int m;
	fSuppressMap=0;
	for (m=0; mode[m]!=0; m++)
	{
		if (mode[m]=='u')
		{
			fSuppressMap=1;
			break;
		}
	}
	tif = TIFFClientOpen(name, mode, (thandle_t)ifd, /* FIXME: WIN64 cast to pointer warning */
			_tiffReadProc, _tiffWriteProc,
			_tiffSeekProc, _tiffCloseProc, _tiffSizeProc,
			fSuppressMap ? _tiffDummyMapProc : _tiffMapProc,
			fSuppressMap ? _tiffDummyUnmapProc : _tiffUnmapProc);
	if (tif)
		tif->tif_fd = ifd;
	return (tif);
}