EmpathyAdiumData  *
empathy_adium_data_new (const gchar *path)
{
	EmpathyAdiumData *data;
	GHashTable *info;

	info = empathy_adium_info_new (path);
	data = empathy_adium_data_new_with_info (path, info);
	g_hash_table_unref (info);

	return data;