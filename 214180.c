static int rar5_has_encrypted_entries(struct archive_read *_a) {
	(void) _a;

	/* Unsupported for now. */
	return ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED;
}