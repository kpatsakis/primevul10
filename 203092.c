theme_adium_load_finished_cb (WebKitWebView  *view,
			      WebKitWebFrame *frame,
			      gpointer        user_data)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (view);
	EmpathyChatView       *chat_view = EMPATHY_CHAT_VIEW (view);
	GList                 *l;

	DEBUG ("Page loaded");
	priv->pages_loading--;

	if (priv->pages_loading != 0)
		return;

	/* Display queued messages */
	for (l = priv->message_queue.head; l != NULL; l = l->next) {
		QueuedItem *item = l->data;

		switch (item->type)
		{
			case QUEUED_MESSAGE:
				theme_adium_append_message (chat_view, item->msg);
				break;

			case QUEUED_EDIT:
				theme_adium_edit_message (chat_view, item->msg);
				break;

			case QUEUED_EVENT:
				theme_adium_append_event (chat_view, item->str);
				break;
		}

		free_queued_item (item);
	}

	g_queue_clear (&priv->message_queue);
}