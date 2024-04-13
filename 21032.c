fr_window_free_private_data (FrWindow *window)
{
	if (window->priv->update_timeout_handle != 0) {
		g_source_remove (window->priv->update_timeout_handle);
		window->priv->update_timeout_handle = 0;
	}

	if (window->priv->activity_timeout_handle != 0) {
		g_source_remove (window->priv->activity_timeout_handle);
		window->priv->activity_timeout_handle = 0;
	}

	if (window->priv->progress_timeout != 0) {
		g_source_remove (window->priv->progress_timeout);
		window->priv->progress_timeout = 0;
	}

	if (window->priv->hide_progress_timeout != 0) {
		g_source_remove (window->priv->hide_progress_timeout);
		window->priv->hide_progress_timeout = 0;
	}

	if (window->priv->theme_changed_handler_id != 0)
		g_signal_handler_disconnect (gtk_icon_theme_get_default (), window->priv->theme_changed_handler_id);

	fr_window_history_clear (window);

	_g_object_unref (window->priv->open_default_dir);
	_g_object_unref (window->priv->add_default_dir);
	_g_object_unref (window->priv->extract_default_dir);
	_g_object_unref (window->priv->archive_file);
	_g_object_unref (window->priv->last_extraction_destination);

	g_free (window->priv->password);
	g_free (window->priv->second_password);
	g_free (window->priv->custom_action_message);

	g_object_unref (window->priv->list_store);

	if (window->priv->clipboard_data != NULL) {
		fr_clipboard_data_unref (window->priv->clipboard_data);
		window->priv->clipboard_data = NULL;
	}
	if (window->priv->copy_data != NULL) {
		fr_clipboard_data_unref (window->priv->copy_data);
		window->priv->copy_data = NULL;
	}
	if (window->priv->copy_from_archive != NULL) {
		g_object_unref (window->priv->copy_from_archive);
		window->priv->copy_from_archive = NULL;
	}

	_g_object_unref (window->priv->saving_file);

	fr_window_free_open_files (window);

	g_clear_error (&window->priv->drag_error);
	_g_string_list_free (window->priv->drag_file_list);
	window->priv->drag_file_list = NULL;

	if (window->priv->file_popup_menu != NULL) {
		gtk_widget_destroy (window->priv->file_popup_menu);
		window->priv->file_popup_menu = NULL;
	}

	if (window->priv->folder_popup_menu != NULL) {
		gtk_widget_destroy (window->priv->folder_popup_menu);
		window->priv->folder_popup_menu = NULL;
	}

	if (window->priv->sidebar_folder_popup_menu != NULL) {
		gtk_widget_destroy (window->priv->sidebar_folder_popup_menu);
		window->priv->sidebar_folder_popup_menu = NULL;
	}

	g_free (window->priv->last_location);

	fr_window_free_batch_data (window);
	g_free (window->priv->batch_title);

	_g_object_unref (window->priv->pd_last_archive);
	g_free (window->priv->pd_last_message);

	g_settings_set_enum (window->priv->settings_listing, PREF_LISTING_LIST_MODE, window->priv->last_list_mode);

	_g_object_unref (window->priv->settings_listing);
	_g_object_unref (window->priv->settings_ui);
	_g_object_unref (window->priv->settings_general);
	_g_object_unref (window->priv->settings_dialogs);
	_g_object_unref (window->priv->settings_nautilus);

	_g_object_unref (window->priv->cancellable);
	g_hash_table_unref (window->priv->named_dialogs);
	g_object_unref (window->priv->window_group);
}