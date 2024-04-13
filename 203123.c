message_request_queued_cb (SoupSession *session, SoupMessage *message, MessageData *data)
{
	if (message == data->message) {
#if GLIB_CHECK_VERSION (2, 31, 0)
		g_mutex_unlock (&(data->mutex));
#else
		g_static_mutex_unlock (&(data->mutex));
#endif
	}
}