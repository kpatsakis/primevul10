_fr_window_notify_creation_complete (FrWindow *window)
{
	char               *basename;
	char               *message;
	NotifyNotification *notification;
	gboolean            notification_supports_actions;
	GList              *caps;
	NotifyData         *notify_data;

	basename = _g_file_get_display_basename (window->priv->saving_file);
	/* Translators: %s is a filename */
	message = g_strdup_printf (_("\"%s\" created successfully"), basename);
	notification = notify_notification_new (window->priv->batch_title, message, "file-roller");
	notify_notification_set_hint_string (notification, "desktop-entry", "file-roller");

	notify_data = g_new0 (NotifyData, 1);
	notify_data->window = window;
	notify_data->window_closed = FALSE;
	g_signal_connect (notification,
			  "closed",
			  G_CALLBACK (notification_closed_cb),
			  notify_data);

	notification_supports_actions = FALSE;
	caps = notify_get_server_caps ();
	if (caps != NULL) {
		notification_supports_actions = g_list_find_custom (caps, "actions", (GCompareFunc) strcmp) != NULL;
		_g_string_list_free (caps);
	}

	if (notification_supports_actions) {
		notify_notification_add_action (notification,
						"document-open-symbolic",
						_("Open"),
						notify_action_open_archive_cb,
						notify_data,
						NULL);
		/*notify_notification_set_hint (notification,
					      "action-icons",
					      g_variant_new_boolean (TRUE));*/
	}

	notify_notification_show (notification, NULL);
	g_free (message);
	g_free (basename);
}