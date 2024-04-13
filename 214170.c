static int skip_unprocessed_bytes(struct archive_read* a) {
	struct rar5* rar = get_context(a);
	int ret;

	if(rar->file.bytes_remaining) {
		/* Use different skipping method in block merging mode than in
		 * normal mode. If merge mode is active, rar5_read_data_skip
		 * can't be used, because it could allow recursive use of
		 * merge_block() * function, and this function doesn't support
		 * recursive use. */
		if(rar->merge_mode) {
			/* Discard whole merged block. This is valid in solid
			 * mode as well, because the code will discard blocks
			 * only if those blocks are safe to discard (i.e.
			 * they're not FILE blocks).  */
			ret = consume(a, rar->file.bytes_remaining);
			if(ret != ARCHIVE_OK) {
				return ret;
			}
			rar->file.bytes_remaining = 0;
		} else {
			/* If we're not in merge mode, use safe skipping code.
			 * This will ensure we'll handle solid archives
			 * properly. */
			ret = rar5_read_data_skip(a);
			if(ret != ARCHIVE_OK) {
				return ret;
			}
		}
	}

	return ARCHIVE_OK;
}