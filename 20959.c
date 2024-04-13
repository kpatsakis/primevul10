fr_window_init (FrWindow *window)
{
	window->priv = g_new0 (FrWindowPrivate, 1);
	window->priv->update_dropped_files = FALSE;
	window->priv->filter_mode = FALSE;
	window->priv->use_progress_dialog = TRUE;
	window->priv->batch_title = NULL;
	window->priv->cancellable = g_cancellable_new ();
	window->priv->compression = FR_COMPRESSION_NORMAL;
	window->priv->window_group = gtk_window_group_new ();
	window->priv->populating_file_list = FALSE;
	window->priv->named_dialogs = g_hash_table_new (g_str_hash, g_str_equal);
	gtk_window_group_add_window (window->priv->window_group, GTK_WINDOW (window));

	window->archive = NULL;
}