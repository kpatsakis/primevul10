fr_window_set_batch__extract_here (FrWindow *window,
				   GFile    *archive)
{
	g_return_if_fail (window != NULL);
	g_return_if_fail (archive != NULL);

	fr_window_append_batch_action (window,
				       FR_BATCH_ACTION_LOAD,
				       g_object_ref (archive),
				       (GFreeFunc) g_object_unref);
	fr_window_append_batch_action (window,
				       FR_BATCH_ACTION_EXTRACT_HERE,
				       extract_to_data_new (window, NULL),
				       (GFreeFunc) extract_data_free);
	fr_window_append_batch_action (window,
				       FR_BATCH_ACTION_CLOSE,
				       NULL,
				       NULL);
}