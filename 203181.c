message_cancel_cb (GCancellable *cancellable, MessageData *data)
{
#if GLIB_CHECK_VERSION (2, 31, 0)
	g_mutex_lock (&(data->mutex));
	soup_session_cancel_message (data->session, data->message, SOUP_STATUS_CANCELLED);
	g_mutex_unlock (&(data->mutex));
#else
	g_static_mutex_lock (&(data->mutex));
	soup_session_cancel_message (data->session, data->message, SOUP_STATUS_CANCELLED);
	g_static_mutex_unlock (&(data->mutex));
#endif
}