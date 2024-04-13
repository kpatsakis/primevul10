DEFINE_TEST(test_read_format_rar5_readtables_overflow)
{
	uint8_t buf[16];

	PROLOGUE("test_read_format_rar5_readtables_overflow.rar");

	/* This archive is invalid. However, processing it shouldn't cause any
	 * buffer overflow errors during reading rar5 tables. */

	(void) archive_read_next_header(a, &ae);
	(void) archive_read_data(a, buf, sizeof(buf));
	(void) archive_read_next_header(a, &ae);

	EPILOGUE();
}