empathy_adium_info_new (const gchar *path)
{
	gchar *file;
	GValue *value;
	GHashTable *info = NULL;

	g_return_val_if_fail (empathy_adium_path_is_valid (path), NULL);

	file = g_build_filename (path, "Contents", "Info.plist", NULL);
	value = empathy_plist_parse_from_file (file);
	g_free (file);

	if (value == NULL)
		return NULL;

	info = g_value_dup_boxed (value);
	tp_g_value_slice_free (value);

	/* Insert the theme's path into the hash table,
	 * keys have to be dupped */
	tp_asv_set_string (info, g_strdup ("path"), path);

	return info;
}