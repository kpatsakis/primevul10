_gdata_service_actually_send_message (SoupSession *session, SoupMessage *message, GCancellable *cancellable, GError **error)
{
	MessageData data;
	gulong cancel_signal = 0, request_queued_signal = 0;

	/* Hold references to the session and message so they can't be freed by other threads. For example, if the SoupSession was freed by another
	 * thread while we were making a request, the request would be unexpectedly cancelled. See bgo#650835 for an example of this breaking things.
	 */
	g_object_ref (session);
	g_object_ref (message);

	/* Listen for cancellation */
	if (cancellable != NULL) {
#if GLIB_CHECK_VERSION (2, 31, 0)
		g_mutex_init (&(data.mutex));
#else
		g_static_mutex_init (&(data.mutex));
#endif
		data.session = session;
		data.message = message;

		cancel_signal = g_cancellable_connect (cancellable, (GCallback) message_cancel_cb, &data, NULL);
		request_queued_signal = g_signal_connect (session, "request-queued", (GCallback) message_request_queued_cb, &data);

		/* We lock this mutex until the message has been queued by the session (i.e. it's unlocked in the request-queued callback), and require
		 * the mutex to be held to cancel the message. Consequently, if the message is cancelled (in another thread) any time between this lock
		 * and the request being queued, the cancellation will wait until the request has been queued before taking effect.
		 * This is a little ugly, but is the only way I can think of to avoid a race condition between calling soup_session_cancel_message()
		 * and soup_session_send_message(), as the former doesn't have any effect until the request has been queued, and once the latter has
		 * returned, all network activity has been finished so cancellation is pointless. */
#if GLIB_CHECK_VERSION (2, 31, 0)
		g_mutex_lock (&(data.mutex));
#else
		g_static_mutex_lock (&(data.mutex));
#endif
	}

	/* Only send the message if it hasn't already been cancelled. There is no race condition here for the above reasons: if the cancellable has
	 * been cancelled, it's because it was cancelled before we called g_cancellable_connect().
	 *
	 * Otherwise, manually set the message's status code to SOUP_STATUS_CANCELLED, as the message was cancelled before even being queued to be
	 * sent. */
	if (cancellable == NULL || g_cancellable_is_cancelled (cancellable) == FALSE)
		soup_session_send_message (session, message);
	else
		soup_message_set_status (message, SOUP_STATUS_CANCELLED);

	/* Clean up the cancellation code */
	if (cancellable != NULL) {
		g_signal_handler_disconnect (session, request_queued_signal);

		if (cancel_signal != 0)
			g_cancellable_disconnect (cancellable, cancel_signal);

#if GLIB_CHECK_VERSION (2, 31, 0)
		g_mutex_clear (&(data.mutex));
#else
		g_static_mutex_free (&(data.mutex));
#endif
	}

	/* Set the cancellation error if applicable. We can't assume that our GCancellable has been cancelled just because the message has;
	 * libsoup may internally cancel messages if, for example, the proxy URI of the SoupSession is changed.
	 * libsoup also sometimes seems to return a SOUP_STATUS_IO_ERROR when we cancel a message, even though we've specified SOUP_STATUS_CANCELLED
	 * at cancellation time. Ho Hum. */
	g_assert (message->status_code != SOUP_STATUS_NONE);

	if (message->status_code == SOUP_STATUS_CANCELLED ||
	    (message->status_code == SOUP_STATUS_IO_ERROR && cancellable != NULL && g_cancellable_is_cancelled (cancellable) == TRUE)) {
		/* We hackily create and cancel a new GCancellable so that we can set the error using it and therefore save ourselves a translatable
		 * string and the associated maintenance. */
		GCancellable *error_cancellable = g_cancellable_new ();
		g_cancellable_cancel (error_cancellable);
		g_assert (g_cancellable_set_error_if_cancelled (error_cancellable, error) == TRUE);
		g_object_unref (error_cancellable);

		/* As per the above comment, force the status to be SOUP_STATUS_CANCELLED. */
		soup_message_set_status (message, SOUP_STATUS_CANCELLED);
	}

	/* Free things */
	g_object_unref (message);
	g_object_unref (session);
}