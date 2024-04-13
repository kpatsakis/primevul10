_tiffSeekProc(thandle_t fd, uint64 off, int whence)
{
	fd_as_handle_union_t fdh;
	_TIFF_off_t off_io = (_TIFF_off_t) off;
	if ((uint64) off_io != off)
	{
		errno=EINVAL;
		return (uint64) -1; /* this is really gross */
	}
	fdh.h = fd;
	return((uint64)_TIFF_lseek_f(fdh.fd,off_io,whence));
}