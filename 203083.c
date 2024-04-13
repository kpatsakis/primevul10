void
empathy_adium_data_unref (EmpathyAdiumData *data)
{
	g_return_if_fail (data != NULL);

	if (g_atomic_int_dec_and_test (&data->ref_count)) {
		g_free (data->path);
		g_free (data->basedir);
		g_free (data->default_avatar_filename);
		g_free (data->default_incoming_avatar_filename);
		g_free (data->default_outgoing_avatar_filename);
		g_hash_table_unref (data->info);
		g_ptr_array_unref (data->strings_to_free);
		tp_clear_pointer (&data->date_format_cache, g_hash_table_unref);

		g_slice_free (EmpathyAdiumData, data);
	}