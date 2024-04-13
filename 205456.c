_tiffCloseProc(thandle_t fd)
{
	fd_as_handle_union_t fdh;
	fdh.h = fd;
	return(close(fdh.fd));
}