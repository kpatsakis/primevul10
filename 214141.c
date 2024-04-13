static inline int get_archive_read(struct archive* a,
    struct archive_read** ar)
{
	*ar = (struct archive_read*) a;
	archive_check_magic(a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_NEW,
	    "archive_read_support_format_rar5");

	return ARCHIVE_OK;
}