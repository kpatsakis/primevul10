get_dir_list_from_path (FrWindow *window,
	      		char     *path)
{
	char  *dirname;
	int    dirname_l;
	GList *list = NULL;
	int    i;

	if (path[strlen (path) - 1] != '/')
		dirname = g_strconcat (path, "/", NULL);
	else
		dirname = g_strdup (path);
	dirname_l = strlen (dirname);
	for (i = 0; i < window->archive->files->len; i++) {
		FileData *fd = g_ptr_array_index (window->archive->files, i);
		gboolean  matches = FALSE;

#ifdef DEBUG_GET_DIR_LIST_FROM_PATH
		g_print ("%s <=> %s (%d)\n", dirname, fd->full_path, dirname_l);
#endif

		if (fd->dir) {
			int full_path_l = strlen (fd->full_path);
			if ((full_path_l == dirname_l - 1) && (strncmp (dirname, fd->full_path, full_path_l) == 0))
				/* example: dirname is '/path/to/dir/' and fd->full_path is '/path/to/dir' */
				matches = TRUE;
			else if (strcmp (dirname, fd->full_path) == 0)
				matches = TRUE;
		}

		if (! matches && strncmp (dirname, fd->full_path, dirname_l) == 0) {
			matches = TRUE;
		}

		if (matches) {
#ifdef DEBUG_GET_DIR_LIST_FROM_PATH
			g_print ("`-> OK\n");
#endif
			list = g_list_prepend (list, g_strdup (fd->original_path));
		}
	}
	g_free (dirname);

	return g_list_reverse (list);
}