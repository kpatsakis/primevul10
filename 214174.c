static int parse_file_extra_redir(struct archive_read* a,
    struct archive_entry* e, struct rar5* rar, ssize_t* extra_data_size)
{
	uint64_t value_size = 0;
	size_t target_size = 0;
	char target_utf8_buf[MAX_NAME_IN_BYTES];
	const uint8_t* p;

	if(!read_var(a, &rar->file.redir_type, &value_size))
		return ARCHIVE_EOF;
	if(ARCHIVE_OK != consume(a, (int64_t)value_size))
		return ARCHIVE_EOF;
	*extra_data_size -= value_size;

	if(!read_var(a, &rar->file.redir_flags, &value_size))
		return ARCHIVE_EOF;
	if(ARCHIVE_OK != consume(a, (int64_t)value_size))
		return ARCHIVE_EOF;
	*extra_data_size -= value_size;

	if(!read_var_sized(a, &target_size, NULL))
		return ARCHIVE_EOF;
	*extra_data_size -= target_size + 1;

	if(!read_ahead(a, target_size, &p))
		return ARCHIVE_EOF;

	if(target_size > (MAX_NAME_IN_CHARS - 1)) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Link target is too long");
		return ARCHIVE_FATAL;
	}

	if(target_size == 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "No link target specified");
		return ARCHIVE_FATAL;
	}

	memcpy(target_utf8_buf, p, target_size);
	target_utf8_buf[target_size] = 0;

	if(ARCHIVE_OK != consume(a, (int64_t)target_size))
		return ARCHIVE_EOF;

	switch(rar->file.redir_type) {
		case REDIR_TYPE_UNIXSYMLINK:
		case REDIR_TYPE_WINSYMLINK:
			archive_entry_set_filetype(e, AE_IFLNK);
			archive_entry_update_symlink_utf8(e, target_utf8_buf);
			if (rar->file.redir_flags & REDIR_SYMLINK_IS_DIR) {
				archive_entry_set_symlink_type(e,
					AE_SYMLINK_TYPE_DIRECTORY);
			} else {
				archive_entry_set_symlink_type(e,
				AE_SYMLINK_TYPE_FILE);
			}
			break;

		case REDIR_TYPE_HARDLINK:
			archive_entry_set_filetype(e, AE_IFREG);
			archive_entry_update_hardlink_utf8(e, target_utf8_buf);
			break;

		default:
			/* Unknown redir type, skip it. */
			break;
	}
	return ARCHIVE_OK;
}