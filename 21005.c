save_archive_thread (GSimpleAsyncResult *result,
		     GObject            *object,
		     GCancellable       *cancellable)
{
	SaveData             *save_data;
	LoadData             *load_data;
	struct archive       *a, *b;
	struct archive_entry *r_entry;
	int                   ra, rb;

	save_data = g_simple_async_result_get_op_res_gpointer (result);
	load_data = LOAD_DATA (save_data);

	save_data->b = b = archive_write_new ();
	_archive_write_set_format_from_context (b, save_data);
	archive_write_open (b, save_data, save_data_open, save_data_write, save_data_close);
	archive_write_set_bytes_in_last_block (b, 1);

	a = archive_read_new ();
	archive_read_support_filter_all (a);
	archive_read_support_format_all (a);
	archive_read_open (a, load_data, load_data_open, load_data_read, load_data_close);

	if (save_data->begin_operation != NULL)
		save_data->begin_operation (save_data, save_data->user_data);

	while ((load_data->error == NULL) && (ra = archive_read_next_header (a, &r_entry)) == ARCHIVE_OK) {
		struct archive_entry *w_entry;
		WriteAction           action;

		if (g_cancellable_is_cancelled (cancellable))
			break;

		action = WRITE_ACTION_WRITE_ENTRY;
		w_entry = archive_entry_clone (r_entry);
		if (save_data->entry_action != NULL)
			action = save_data->entry_action (save_data, w_entry, save_data->user_data);

		if (action == WRITE_ACTION_WRITE_ENTRY) {
			const void   *buffer;
			size_t        buffer_size;
			__LA_INT64_T  offset;

			rb = archive_write_header (b, w_entry);
			if (rb != ARCHIVE_OK)
				break;

			switch (archive_entry_filetype (r_entry)) {
			case AE_IFREG:
				while ((ra = archive_read_data_block (a, &buffer, &buffer_size, &offset)) == ARCHIVE_OK) {
					archive_write_data (b, buffer, buffer_size);
					fr_archive_progress_inc_completed_bytes (load_data->archive, buffer_size);
				}

				if (ra != ARCHIVE_EOF)
					load_data->error = g_error_new_literal (FR_ERROR, FR_ERROR_COMMAND_ERROR, archive_error_string (a));
				break;

			default:
				break;
			}

			rb = archive_write_finish_entry (b);
		}
		else if (action == WRITE_ACTION_SKIP_ENTRY)
			fr_archive_progress_inc_completed_bytes (load_data->archive, archive_entry_size (r_entry));

		archive_entry_free (w_entry);
	}

	if (g_error_matches (load_data->error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
		ra = ARCHIVE_EOF;

	if (save_data->end_operation != NULL)
		save_data->end_operation (save_data, save_data->user_data);

	rb = archive_write_close (b);

	if ((load_data->error == NULL) && (ra != ARCHIVE_EOF))
		load_data->error = g_error_new_literal (FR_ERROR, FR_ERROR_COMMAND_ERROR, archive_error_string (a));
	if ((load_data->error == NULL) && (rb != ARCHIVE_OK))
		load_data->error = g_error_new_literal (FR_ERROR, FR_ERROR_COMMAND_ERROR, archive_error_string (b));
	if (load_data->error == NULL)
		g_cancellable_set_error_if_cancelled (cancellable, &load_data->error);
	if (load_data->error != NULL)
		g_simple_async_result_set_from_error (result, load_data->error);

	archive_read_free (a);
	archive_write_free (b);
	save_data_free (save_data);
}