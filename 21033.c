fr_window_free_batch_data (FrWindow *window)
{
	GList *scan;

	for (scan = window->priv->batch_action_list; scan; scan = scan->next) {
		FrBatchAction *adata = scan->data;

		if ((adata->data != NULL) && (adata->free_func != NULL))
			(*adata->free_func) (adata->data);
		g_free (adata);
	}

	g_list_free (window->priv->batch_action_list);
	window->priv->batch_action_list = NULL;
	window->priv->batch_action = NULL;

	fr_window_reset_current_batch_action (window);
}