TIFFFdOpen(int fd, const char* name, const char* mode)
{
	TIFF* tif;

	fd_as_handle_union_t fdh;
	fdh.fd = fd;
	tif = TIFFClientOpen(name, mode,
	    fdh.h,
	    _tiffReadProc, _tiffWriteProc,
	    _tiffSeekProc, _tiffCloseProc, _tiffSizeProc,
	    _tiffMapProc, _tiffUnmapProc);
	if (tif)
		tif->tif_fd = fd;
	return (tif);
}