adium_info_dup_path_for_variant (GHashTable *info,
				 const gchar *variant)
{
	guint version = adium_info_get_version (info);
	const gchar *no_variant = adium_info_get_no_variant_name (info);
	GPtrArray *variants;
	guint i;

	if (version <= 2 && !tp_strdiff (variant, no_variant)) {
		return g_strdup ("main.css");
	}

	variants = empathy_adium_info_get_available_variants (info);
	if (variants->len == 0)
		return g_strdup ("main.css");

	/* Verify the variant exists, fallback to the first one */
	for (i = 0; i < variants->len; i++) {
		if (!tp_strdiff (variant, g_ptr_array_index (variants, i))) {
			break;
		}
	}
	if (i == variants->len) {
		DEBUG ("Variant %s does not exist", variant);
		variant = g_ptr_array_index (variants, 0);
	}

	return g_strdup_printf ("Variants/%s.css", variant);

}