static int parse_file_extra_hash(struct archive_read* a, struct rar5* rar,
    ssize_t* extra_data_size)
{
	size_t hash_type = 0;
	size_t value_len;

	enum HASH_TYPE {
		BLAKE2sp = 0x00
	};

	if(!read_var_sized(a, &hash_type, &value_len))
		return ARCHIVE_EOF;

	*extra_data_size -= value_len;
	if(ARCHIVE_OK != consume(a, value_len)) {
		return ARCHIVE_EOF;
	}

	/* The file uses BLAKE2sp checksum algorithm instead of plain old
	 * CRC32. */
	if(hash_type == BLAKE2sp) {
		const uint8_t* p;
		const int hash_size = sizeof(rar->file.blake2sp);

		if(!read_ahead(a, hash_size, &p))
			return ARCHIVE_EOF;

		rar->file.has_blake2 = 1;
		memcpy(&rar->file.blake2sp, p, hash_size);

		if(ARCHIVE_OK != consume(a, hash_size)) {
			return ARCHIVE_EOF;
		}

		*extra_data_size -= hash_size;
	} else {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unsupported hash type (0x%x)", (int) hash_type);
		return ARCHIVE_FATAL;
	}

	return ARCHIVE_OK;
}