adium_info_get_no_variant_name (GHashTable *info)
{
	const gchar *name = tp_asv_get_string (info, "DisplayNameForNoVariant");
	return name ? name : _("Normal");
}