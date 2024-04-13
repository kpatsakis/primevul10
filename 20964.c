fr_window_unrealize (GtkWidget *widget)
{
	FrWindow     *window = FR_WINDOW (widget);
	GtkClipboard *clipboard;

	gth_icon_cache_free (window->priv->list_icon_cache);
	window->priv->list_icon_cache = NULL;

	gth_icon_cache_free (window->priv->tree_icon_cache);
	window->priv->tree_icon_cache = NULL;

	clipboard = gtk_widget_get_clipboard (widget, FR_CLIPBOARD);
	g_signal_handlers_disconnect_by_func (clipboard,
					      G_CALLBACK (clipboard_owner_change_cb),
					      window);

	GTK_WIDGET_CLASS (fr_window_parent_class)->unrealize (widget);
}