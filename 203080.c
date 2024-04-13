theme_adium_message_acknowledged (EmpathyChatView *view,
				  EmpathyMessage  *message)
{
	EmpathyThemeAdium *self = (EmpathyThemeAdium *) view;
	EmpathyThemeAdiumPriv *priv = GET_PRIV (view);
	TpMessage *tp_msg;
	guint32 id;
	gboolean valid;

	tp_msg = empathy_message_get_tp_message (message);

	if (tp_msg == NULL) {
		return;
	}

	id = tp_message_get_pending_message_id (tp_msg, &valid);
	if (!valid) {
		g_warning ("Acknoledged message doesn't have a pending ID");
		return;
	}

	/* We only want to actually remove the unread marker if the
	 * view doesn't have focus. If we did it all the time we would
	 * never see the unread markers, ever! So, we'll queue these
	 * up, and when we lose focus, we'll remove the markers. */
	if (priv->has_focus) {
		g_queue_push_tail (&priv->acked_messages,
				   GUINT_TO_POINTER (id));
		return;
	}

	theme_adium_remove_mark_from_message (self, id);
}