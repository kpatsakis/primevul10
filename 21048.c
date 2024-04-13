fr_window_update_history (FrWindow *window)
{
	GList *scan;

	/* remove the paths not present in the archive */

	for (scan = window->priv->history; scan; /* void */) {
		GList *next = scan->next;
		char  *path = scan->data;

		if (! fr_window_dir_exists_in_archive (window, path)) {
			if (scan == window->priv->history_current)
				window->priv->history_current = NULL;
			window->priv->history = g_list_remove_link (window->priv->history, scan);
			_g_string_list_free (scan);
		}

		scan = next;
	}

	if (window->priv->history_current == NULL)
		window->priv->history_current = window->priv->history;
}