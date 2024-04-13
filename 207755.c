xbstream_deinit(ds_ctxt_t *ctxt)
{
	ds_stream_ctxt_t	*stream_ctxt;

	stream_ctxt = (ds_stream_ctxt_t *) ctxt->ptr;

	if (xb_stream_write_done(stream_ctxt->xbstream)) {
		msg("xb_stream_done() failed.");
	}

	if (stream_ctxt->dest_file) {
		ds_close(stream_ctxt->dest_file);
		stream_ctxt->dest_file = NULL;
	}

	pthread_mutex_destroy(&stream_ctxt->mutex);

	my_free(ctxt);
}