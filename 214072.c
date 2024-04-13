DEFINE_TEST(test_read_format_rar5_invalid_dict_reference)
{
	uint8_t buf[16];

	PROLOGUE("test_read_format_rar5_invalid_dict_reference.rar");

	/* This test should fail on parsing the header. */
	assertA(archive_read_next_header(a, &ae) != ARCHIVE_OK);

	/* This archive is invalid. However, processing it shouldn't cause any
	 * errors related to buffer underflow when using -fsanitize. */
	assertA(archive_read_data(a, buf, sizeof(buf)) <= 0);

	/* This test only cares about not returning success here. */
	assertA(ARCHIVE_OK != archive_read_next_header(a, &ae));

	EPILOGUE();
}