static int rar5_read_header(struct archive_read *a,
    struct archive_entry *entry)
{
	struct rar5* rar = get_context(a);
	int ret;

	if(rar->header_initialized == 0) {
		init_header(a);
		rar->header_initialized = 1;
	}

	if(rar->skipped_magic == 0) {
		if(ARCHIVE_OK != consume(a, rar5_signature_size)) {
			return ARCHIVE_EOF;
		}

		rar->skipped_magic = 1;
	}

	do {
		ret = process_base_block(a, entry);
	} while(ret == ARCHIVE_RETRY ||
			(rar->main.endarc > 0 && ret == ARCHIVE_OK));

	return ret;
}