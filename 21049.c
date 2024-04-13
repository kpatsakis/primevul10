fr_window_history_add (FrWindow   *window,
		       const char *path)
{
	if ((window->priv->history_current == NULL) || (g_strcmp0 (path, window->priv->history_current->data) != 0)) {
		GList *scan;
		GList *new_current = NULL;

                /* search the path in the history */
                for (scan = window->priv->history_current; scan; scan = scan->next) {
                        char *path_in_history = scan->data;

                        if (g_strcmp0 (path, path_in_history) == 0) {
                        	new_current = scan;
                        	break;
                        }
                }

                if (new_current != NULL) {
                	window->priv->history_current = new_current;
                }
                else {
			/* remove all the paths after the current position */
			for (scan = window->priv->history; scan && (scan != window->priv->history_current); /* void */) {
				GList *next = scan->next;

				window->priv->history = g_list_remove_link (window->priv->history, scan);
				_g_string_list_free (scan);

				scan = next;
			}

			window->priv->history = g_list_prepend (window->priv->history, g_strdup (path));
			window->priv->history_current = window->priv->history;
                }
	}
}