my_xbstream_write_callback(xb_wstream_file_t *f __attribute__((unused)),
		       void *userdata, const void *buf, size_t len)
{
	ds_stream_ctxt_t	*stream_ctxt;

	stream_ctxt = (ds_stream_ctxt_t *) userdata;

	xb_ad(stream_ctxt != NULL);
	xb_ad(stream_ctxt->dest_file != NULL);

	if (!ds_write(stream_ctxt->dest_file, buf, len)) {
		return len;
	}
	return -1;
}