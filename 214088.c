DEFINE_TEST(test_read_format_rar5_different_window_size)
{
	char buf[4096];
	PROLOGUE("test_read_format_rar5_different_window_size.rar");

	/* Return codes of those calls are ignored, because this sample file
	 * is invalid. However, the unpacker shouldn't produce any SIGSEGV
	 * errors during processing. */

	(void) archive_read_next_header(a, &ae);
	while(0 < archive_read_data(a, buf, sizeof(buf))) {}

	(void) archive_read_next_header(a, &ae);
	while(0 < archive_read_data(a, buf, sizeof(buf))) {}

	(void) archive_read_next_header(a, &ae);
	while(0 < archive_read_data(a, buf, sizeof(buf))) {}

	EPILOGUE();
}