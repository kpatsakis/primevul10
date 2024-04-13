GHashTable *
empathy_adium_data_get_info (EmpathyAdiumData *data)
{
	g_return_val_if_fail (data != NULL, NULL);

	return data->info;