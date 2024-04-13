static int process_head_file_extra(struct archive_read* a,
    struct archive_entry* e, struct rar5* rar, ssize_t extra_data_size)
{
	size_t extra_field_size;
	size_t extra_field_id = 0;
	int ret = ARCHIVE_FATAL;
	size_t var_size;

	while(extra_data_size > 0) {
		if(!read_var_sized(a, &extra_field_size, &var_size))
			return ARCHIVE_EOF;

		extra_data_size -= var_size;
		if(ARCHIVE_OK != consume(a, var_size)) {
			return ARCHIVE_EOF;
		}

		if(!read_var_sized(a, &extra_field_id, &var_size))
			return ARCHIVE_EOF;

		extra_data_size -= var_size;
		if(ARCHIVE_OK != consume(a, var_size)) {
			return ARCHIVE_EOF;
		}

		switch(extra_field_id) {
			case EX_HASH:
				ret = parse_file_extra_hash(a, rar,
				    &extra_data_size);
				break;
			case EX_HTIME:
				ret = parse_file_extra_htime(a, e, rar,
				    &extra_data_size);
				break;
			case EX_REDIR:
				ret = parse_file_extra_redir(a, e, rar,
				    &extra_data_size);
				break;
			case EX_UOWNER:
				ret = parse_file_extra_owner(a, e,
				    &extra_data_size);
				break;
			case EX_VERSION:
				ret = parse_file_extra_version(a, e,
				    &extra_data_size);
				break;
			case EX_CRYPT:
				/* fallthrough */
			case EX_SUBDATA:
				/* fallthrough */
			default:
				/* Skip unsupported entry. */
				return consume(a, extra_data_size);
		}
	}

	if(ret != ARCHIVE_OK) {
		/* Attribute not implemented. */
		return ret;
	}

	return ARCHIVE_OK;
}