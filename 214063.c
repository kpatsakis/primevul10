static int skip_base_block(struct archive_read* a) {
	int ret;
	struct rar5* rar = get_context(a);

	/* Create a new local archive_entry structure that will be operated on
	 * by header reader; operations on this archive_entry will be discarded.
	 */
	struct archive_entry* entry = archive_entry_new();
	ret = process_base_block(a, entry);

	/* Discard operations on this archive_entry structure. */
	archive_entry_free(entry);
	if(ret == ARCHIVE_FATAL)
		return ret;

	if(rar->generic.last_header_id == 2 && rar->generic.split_before > 0)
		return ARCHIVE_OK;

	if(ret == ARCHIVE_OK)
		return ARCHIVE_RETRY;
	else
		return ret;
}