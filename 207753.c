xbstream_init(const char *root __attribute__((unused)))
{
	ds_ctxt_t		*ctxt;
	ds_stream_ctxt_t	*stream_ctxt;
	xb_wstream_t *xbstream;

	ctxt = (ds_ctxt_t *)my_malloc(sizeof(ds_ctxt_t) + sizeof(ds_stream_ctxt_t),
			 MYF(MY_FAE));
	stream_ctxt = (ds_stream_ctxt_t *)(ctxt + 1);

	if (pthread_mutex_init(&stream_ctxt->mutex, NULL)) {
		msg("xbstream_init: pthread_mutex_init() failed.");
		goto err;
	}

	xbstream = xb_stream_write_new();
	if (xbstream == NULL) {
		msg("xb_stream_write_new() failed.");
		goto err;
	}
	stream_ctxt->xbstream = xbstream;
	stream_ctxt->dest_file = NULL;

	ctxt->ptr = stream_ctxt;

	return ctxt;

err:
	my_free(ctxt);
	return NULL;
}