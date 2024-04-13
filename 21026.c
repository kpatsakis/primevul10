notify_action_open_archive_cb (NotifyNotification *notification,
			       char               *action,
			       gpointer            user_data)
{
	NotifyData *notify_data = user_data;
	FrWindow   *window = notify_data->window;
	GtkWidget  *new_window;

	new_window = fr_window_new ();
	gtk_widget_show (new_window);
	fr_window_archive_open (FR_WINDOW (new_window),
				window->priv->saving_file,
				GTK_WINDOW (new_window));

	notify_data->window_closed = TRUE;
	_fr_window_close_after_notification (window);
}