fr_window_get_file_list (FrWindow *window)
{
	GList *list;
	int    i;

	g_return_val_if_fail (window != NULL, NULL);

	list = NULL;
	for (i = 0; i < window->archive->files->len; i++) {
		FileData *fd = g_ptr_array_index (window->archive->files, i);
		list = g_list_prepend (list, g_strdup (fd->original_path));
	}

	return g_list_reverse (list);
}