fr_window_set_batch__extract (FrWindow  *window,
			      GFile     *archive,
			      GFile     *destination)
{
	g_return_if_fail (window != NULL);
	g_return_if_fail (archive != NULL);

	fr_window_append_batch_action (window,
				       FR_BATCH_ACTION_LOAD,
				       g_object_ref (archive),
				       (GFreeFunc) g_object_unref);
	if (destination != NULL)
		fr_window_append_batch_action (window,
					       FR_BATCH_ACTION_EXTRACT,
					       extract_to_data_new (window, destination),
					       (GFreeFunc) extract_data_free);
	else
		fr_window_append_batch_action (window,
					       FR_BATCH_ACTION_EXTRACT_INTERACT,
					       NULL,
					       NULL);
	fr_window_append_batch_action (window,
				       FR_BATCH_ACTION_CLOSE,
				       NULL,
				       NULL);
}