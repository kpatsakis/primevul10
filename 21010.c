fr_window_update_sensitivity (FrWindow *window)
{
	gboolean no_archive;
	gboolean ro;
	gboolean file_op;
	gboolean running;
	gboolean can_store_many_files;
	gboolean sel_not_null;
	gboolean one_file_selected;
	gboolean dir_selected;
	int      n_selected;

	if (window->priv->batch_mode)
		return;

	running              = window->priv->activity_ref > 0;
	no_archive           = (window->archive == NULL) || ! window->priv->archive_present;
	ro                   = ! no_archive && window->archive->read_only;
	file_op              = ! no_archive && ! window->priv->archive_new  && ! running;
	can_store_many_files = (window->archive != NULL) && fr_archive_is_capable_of (window->archive, FR_ARCHIVE_CAN_STORE_MANY_FILES);
	n_selected           = fr_window_get_n_selected_files (window);
	sel_not_null         = n_selected > 0;
	one_file_selected    = n_selected == 1;
	dir_selected         = selection_has_a_dir (window);

	set_sensitive (window, "Add", ! no_archive && ! ro && ! running && can_store_many_files);
	set_sensitive (window, "Add_Toolbar", ! no_archive && ! ro && ! running && can_store_many_files);
	set_sensitive (window, "Copy", ! no_archive && ! ro && ! running && can_store_many_files && sel_not_null && (window->priv->list_mode != FR_WINDOW_LIST_MODE_FLAT));
	set_sensitive (window, "Cut", ! no_archive && ! ro && ! running && can_store_many_files && sel_not_null && (window->priv->list_mode != FR_WINDOW_LIST_MODE_FLAT));
	set_sensitive (window, "Delete", ! no_archive && ! ro && ! window->priv->archive_new && ! running && can_store_many_files);
	set_sensitive (window, "DeselectAll", ! no_archive && sel_not_null);
	set_sensitive (window, "Extract", file_op);
	set_sensitive (window, "Extract_Toolbar", file_op);
	set_sensitive (window, "Find", ! no_archive);
	set_sensitive (window, "New", ! running);
	set_sensitive (window, "Open", ! running);
	set_sensitive (window, "Open_Toolbar", ! running);
	set_sensitive (window, "OpenSelection", file_op && sel_not_null && ! dir_selected);
	set_sensitive (window, "OpenFolder", file_op && one_file_selected && dir_selected);
	set_sensitive (window, "Password", ! running && (window->priv->asked_for_password || (! no_archive && window->archive->propPassword)));
	set_sensitive (window, "Properties", file_op);
	set_sensitive (window, "Close", !running || window->priv->stoppable);
	set_sensitive (window, "Reload", ! (no_archive || running));
	set_sensitive (window, "Rename", ! no_archive && ! ro && ! running && can_store_many_files && one_file_selected);
	set_sensitive (window, "SaveAs", ! no_archive && can_store_many_files && ! running);
	set_sensitive (window, "SelectAll", ! no_archive);
	set_sensitive (window, "TestArchive", ! no_archive && ! running && window->archive->propTest);
	set_sensitive (window, "ViewSelection", file_op && one_file_selected && ! dir_selected);
	set_sensitive (window, "ViewSelection_Toolbar", file_op && one_file_selected && ! dir_selected);

	if (window->priv->progress_dialog != NULL)
		gtk_dialog_set_response_sensitive (GTK_DIALOG (window->priv->progress_dialog),
						   GTK_RESPONSE_OK,
						   running && window->priv->stoppable);

	fr_window_update_paste_command_sensitivity (window, NULL);

	set_sensitive (window, "SelectAll", (window->priv->current_view_length > 0) && (window->priv->current_view_length != n_selected));
	set_sensitive (window, "DeselectAll", n_selected > 0);
	set_sensitive (window, "OpenRecent", ! running);
	set_sensitive (window, "OpenRecent_Toolbar", ! running);

	set_sensitive (window, "ViewFolders", (window->priv->list_mode == FR_WINDOW_LIST_MODE_AS_DIR));

	set_sensitive (window, "ViewAllFiles", ! window->priv->filter_mode);
	set_sensitive (window, "ViewAsFolder", ! window->priv->filter_mode);
}