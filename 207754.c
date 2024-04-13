xbstream_close(ds_file_t *file)
{
	ds_stream_file_t	*stream_file;
	int			rc = 0;

	stream_file = (ds_stream_file_t *)file->ptr;

	rc = xb_stream_write_close(stream_file->xbstream_file);

	my_free(file);

	return rc;
}