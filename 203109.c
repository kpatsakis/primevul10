empathy_adium_info_get_available_variants (GHashTable *info)
{
	GPtrArray *variants;
	const gchar *path;
	gchar *dirpath;
	GDir *dir;

	variants = tp_asv_get_boxed (info, "AvailableVariants", G_TYPE_PTR_ARRAY);
	if (variants != NULL) {
		return variants;
	}

	variants = g_ptr_array_new_with_free_func (g_free);
	tp_asv_take_boxed (info, g_strdup ("AvailableVariants"),
		G_TYPE_PTR_ARRAY, variants);

	path = tp_asv_get_string (info, "path");
	dirpath = g_build_filename (path, "Contents", "Resources", "Variants", NULL);
	dir = g_dir_open (dirpath, 0, NULL);
	if (dir != NULL) {
		const gchar *name;

		for (name = g_dir_read_name (dir);
		     name != NULL;
		     name = g_dir_read_name (dir)) {
			gchar *display_name;

			if (!g_str_has_suffix (name, ".css")) {
				continue;
			}

			display_name = g_strdup (name);
			strstr (display_name, ".css")[0] = '\0';
			g_ptr_array_add (variants, display_name);
		}
		g_dir_close (dir);
	}
	g_free (dirpath);

	if (adium_info_get_version (info) <= 2) {
		g_ptr_array_add (variants,
			g_strdup (adium_info_get_no_variant_name (info)));
	}

	return variants;
}