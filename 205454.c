_tiffMapProc(thandle_t fd, void** pbase, toff_t* psize)
{
	uint64 size64 = _tiffSizeProc(fd);
	tmsize_t sizem = (tmsize_t)size64;
	if ((uint64)sizem==size64) {
		fd_as_handle_union_t fdh;
		fdh.h = fd;
		*pbase = (void*)
		    mmap(0, (size_t)sizem, PROT_READ, MAP_SHARED, fdh.fd, 0);
		if (*pbase != (void*) -1) {
			*psize = (tmsize_t)sizem;
			return (1);
		}
	}
	return (0);
}