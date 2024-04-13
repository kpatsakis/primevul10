_convertion_completed_with_error (FrWindow *window,
				  FrAction  action,
				  GError   *error)
{
	gboolean opens_dialog;

	g_return_if_fail (error != NULL);

#ifdef DEBUG
	debug (DEBUG_INFO, "%s [DONE] (FR::Window)\n", action_names[action]);
#endif

	_fr_window_stop_activity_mode (window);
	close_progress_dialog (window, FALSE);

	_handle_archive_operation_error (window, window->archive, action, error, NULL, &opens_dialog);
	if (opens_dialog)
		return;

	_g_clear_object (&window->priv->saving_file);
	fr_window_stop_batch (window);
}