fr_window_open_files (FrWindow *window,
		      GList    *file_list,
		      gboolean  ask_application)
{
	OpenFilesData *odata;

	if (window->priv->activity_ref > 0)
		return;

	odata = open_files_data_new (window, file_list, ask_application);
	fr_window_set_current_batch_action (window,
					    FR_BATCH_ACTION_OPEN_FILES,
					    odata,
					    (GFreeFunc) open_files_data_unref);

	_archive_operation_started (odata->window, FR_ACTION_EXTRACTING_FILES);

	fr_archive_extract (window->archive,
			    odata->file_list,
			    odata->cdata->temp_dir,
			    NULL,
			    FALSE,
			    TRUE,
			    FALSE,
			    window->priv->password,
			    window->priv->cancellable,
			    open_files_extract_ready_cb,
			    odata);
}