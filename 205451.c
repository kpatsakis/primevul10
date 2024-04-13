_tiffReadProc(thandle_t fd, void* buf, tmsize_t size)
{
	fd_as_handle_union_t fdh;
        const size_t bytes_total = (size_t) size;
        size_t bytes_read;
        tmsize_t count = -1;
	if ((tmsize_t) bytes_total != size)
	{
		errno=EINVAL;
		return (tmsize_t) -1;
	}
	fdh.h = fd;
        for (bytes_read=0; bytes_read < bytes_total; bytes_read+=count)
        {
                char *buf_offset = (char *) buf+bytes_read;
                size_t io_size = bytes_total-bytes_read;
                if (io_size > TIFF_IO_MAX)
                        io_size = TIFF_IO_MAX;
                count=read(fdh.fd, buf_offset, (TIFFIOSize_t) io_size);
                if (count <= 0)
                        break;
        }
        if (count < 0)
                return (tmsize_t)-1;
        return (tmsize_t) bytes_read;
}