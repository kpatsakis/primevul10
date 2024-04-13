_g_path_remove_extension (const gchar *path)
{
	const char *ext;

	ext = _g_filename_get_extension (path);
	if (ext == NULL)
		return g_strdup (path);
	else
		return g_strndup (path, strlen (path) - strlen (ext));
}