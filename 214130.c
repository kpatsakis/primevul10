DEFINE_TEST(test_read_format_rar5_set_format)
{
	struct archive *a;
	struct archive_entry *ae;
	const char reffile[] = "test_read_format_rar5_stored.rar";

	extract_reference_file(reffile);
	assert((a = archive_read_new()) != NULL);
	assertA(0 == archive_read_support_filter_all(a));
	assertA(0 == archive_read_set_format(a, ARCHIVE_FORMAT_RAR_V5));
	assertA(0 == archive_read_open_filename(a, reffile, 10240));
	assertA(0 == archive_read_next_header(a, &ae));
	EPILOGUE();
}