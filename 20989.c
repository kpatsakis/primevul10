fr_window_archive_extract_here (FrWindow   *window,
				gboolean    skip_older,
				gboolean    overwrite,
				gboolean    junk_paths,
				gboolean    ask_to_open_destination)
{
	ExtractData *edata;

	edata = extract_data_new (window,
				  NULL,
				  NULL,
				  NULL,
				  skip_older,
				  overwrite,
				  junk_paths,
				  TRUE,
				  ask_to_open_destination);
	fr_window_set_current_batch_action (window,
					    FR_BATCH_ACTION_EXTRACT,
					    edata,
					    (GFreeFunc) extract_data_free);

	if (archive_is_encrypted (window, NULL) && (window->priv->password == NULL)) {
		dlg_ask_password (window);
		return;
	}

	_archive_operation_started (window, FR_ACTION_EXTRACTING_FILES);

	fr_archive_extract_here (window->archive,
				 edata->skip_older,
				 edata->overwrite,
				 edata->junk_paths,
				 window->priv->password,
				 window->priv->cancellable,
				 archive_extraction_ready_cb,
				 edata);
}