DEFINE_TEST(test_read_format_rar5_truncated_huff)
{
	uint8_t buf[16];

	PROLOGUE("test_read_format_rar5_truncated_huff.rar");

	/* This archive is invalid. However, processing it shouldn't cause any
	 * errors related to undefined operations when using -fsanitize. */

	(void) archive_read_next_header(a, &ae);
	(void) archive_read_data(a, buf, sizeof(buf));
	(void) archive_read_next_header(a, &ae);

	EPILOGUE();
}