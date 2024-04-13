_tiffSizeProc(thandle_t fd)
{
	_TIFF_stat_s sb;
	fd_as_handle_union_t fdh;
	fdh.h = fd;
	if (_TIFF_fstat_f(fdh.fd,&sb)<0)
		return(0);
	else
		return((uint64)sb.st_size);
}