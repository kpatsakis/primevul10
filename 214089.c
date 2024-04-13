static void init_header(struct archive_read* a) {
	a->archive.archive_format = ARCHIVE_FORMAT_RAR_V5;
	a->archive.archive_format_name = "RAR5";
}