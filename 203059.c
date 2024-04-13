adium_info_get_version (GHashTable *info)
{
	return tp_asv_get_int32 (info, "MessageViewVersion", NULL);
}