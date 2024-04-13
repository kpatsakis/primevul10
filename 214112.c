static int process_head_service(struct archive_read* a, struct rar5* rar,
    struct archive_entry* entry, size_t block_flags)
{
	/* Process this SERVICE block the same way as FILE blocks. */
	int ret = process_head_file(a, rar, entry, block_flags);
	if(ret != ARCHIVE_OK)
		return ret;

	rar->file.service = 1;

	/* But skip the data part automatically. It's no use for the user
	 * anyway.  It contains only service data, not even needed to
	 * properly unpack the file. */
	ret = rar5_read_data_skip(a);
	if(ret != ARCHIVE_OK)
		return ret;

	/* After skipping, try parsing another block automatically. */
	return ARCHIVE_RETRY;
}