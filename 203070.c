EmpathyAdiumData  *
empathy_adium_data_ref (EmpathyAdiumData *data)
{
	g_return_val_if_fail (data != NULL, NULL);

	g_atomic_int_inc (&data->ref_count);

	return data;