_archive_write_file (struct archive       *b,
		     SaveData             *save_data,
		     AddFile              *add_file,
		     gboolean              follow_link,
		     struct archive_entry *r_entry,
		     GCancellable         *cancellable)
{
	LoadData             *load_data = LOAD_DATA (save_data);
	GFileInfo            *info;
	struct archive_entry *w_entry;
	int                   rb;

	/* write the file header */

	info = g_file_query_info (add_file->file,
				  FILE_ATTRIBUTES_NEEDED_BY_ARCHIVE_ENTRY,
				  (! follow_link ? G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS : 0),
				  cancellable,
				  &load_data->error);
	if (info == NULL)
		return WRITE_ACTION_ABORT;

	w_entry = archive_entry_new ();
	if (! _archive_entry_copy_file_info (w_entry, info, save_data)) {
		archive_entry_free (w_entry);
		g_object_unref (info);
		return WRITE_ACTION_SKIP_ENTRY;
	}

	/* honor the update flag */

	if (save_data->update && (r_entry != NULL) && (archive_entry_mtime (w_entry) < archive_entry_mtime (r_entry))) {
		archive_entry_free (w_entry);
		g_object_unref (info);
		return WRITE_ACTION_WRITE_ENTRY;
	}

	archive_entry_set_pathname (w_entry, add_file->pathname);
	rb = archive_write_header (b, w_entry);

	/* write the file data */

	if (g_file_info_get_file_type (info) == G_FILE_TYPE_REGULAR) {
		GInputStream *istream;

		istream = (GInputStream *) g_file_read (add_file->file, cancellable, &load_data->error);
		if (istream != NULL) {
			gssize bytes_read;

			while ((bytes_read = g_input_stream_read (istream, save_data->buffer, save_data->buffer_size, cancellable, &load_data->error)) > 0) {
				archive_write_data (b, save_data->buffer, bytes_read);
				fr_archive_progress_inc_completed_bytes (load_data->archive, bytes_read);
			}

			g_object_unref (istream);
		}
	}

	rb = archive_write_finish_entry (b);

	if ((load_data->error == NULL) && (rb != ARCHIVE_OK))
		load_data->error = g_error_new_literal (FR_ERROR, FR_ERROR_COMMAND_ERROR, archive_error_string (b));

	archive_entry_free (w_entry);
	g_object_unref (info);

	return (load_data->error == NULL) ? WRITE_ACTION_SKIP_ENTRY : WRITE_ACTION_ABORT;
}