xbstream_write(ds_file_t *file, const uchar *buf, size_t len)
{
	ds_stream_file_t	*stream_file;
	xb_wstream_file_t	*xbstream_file;


	stream_file = (ds_stream_file_t *) file->ptr;

	xbstream_file = stream_file->xbstream_file;

	if (xb_stream_write_data(xbstream_file, buf, len)) {
		msg("xb_stream_write_data() failed.");
		return 1;
	}

	return 0;
}