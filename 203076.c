empathy_adium_info_get_default_variant (GHashTable *info)
{
	if (adium_info_get_version (info) <= 2) {
		return adium_info_get_no_variant_name (info);
	}

	return tp_asv_get_string (info, "DefaultVariant");
}