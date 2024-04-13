static int64_t rar5_seek_data(struct archive_read *a, int64_t offset,
    int whence)
{
	(void) a;
	(void) offset;
	(void) whence;

	/* We're a streaming unpacker, and we don't support seeking. */

	return ARCHIVE_FATAL;
}