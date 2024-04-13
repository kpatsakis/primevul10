fr_window_finalize (GObject *object)
{
	FrWindow *window = FR_WINDOW (object);

	fr_window_free_open_files (window);

	if (window->archive != NULL) {
		g_object_unref (window->archive);
		window->archive = NULL;
	}

	if (window->priv != NULL) {
		fr_window_free_private_data (window);
		g_free (window->priv);
		window->priv = NULL;
	}

	G_OBJECT_CLASS (fr_window_parent_class)->finalize (object);
}