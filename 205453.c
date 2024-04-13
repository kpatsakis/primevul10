_tiffWriteProc(thandle_t fd, void* buf, tmsize_t size)
{
	fd_as_handle_union_t fdh;
	const size_t bytes_total = (size_t) size;
        size_t bytes_written;
        tmsize_t count = -1;
	if ((tmsize_t) bytes_total != size)
	{
		errno=EINVAL;
		return (tmsize_t) -1;
	}
	fdh.h = fd;
        for (bytes_written=0; bytes_written < bytes_total; bytes_written+=count)
        {
                const char *buf_offset = (char *) buf+bytes_written;
                size_t io_size = bytes_total-bytes_written;
                if (io_size > TIFF_IO_MAX)
                        io_size = TIFF_IO_MAX;
                count=write(fdh.fd, buf_offset, (TIFFIOSize_t) io_size);
                if (count <= 0)
                        break;
        }
        if (count < 0)
                return (tmsize_t)-1;
        return (tmsize_t) bytes_written;
	/* return ((tmsize_t) write(fdh.fd, buf, bytes_total)); */
}