DEFINE_TEST(test_read_format_rar5_multiple_files)
{
	const int DATA_SIZE = 4096;
	uint8_t buff[4096];

	PROLOGUE("test_read_format_rar5_multiple_files.rar");

	/* There should be 4 files inside this test file. Check for their
	 * existence, and also check the contents of those test files. */

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test1.bin", archive_entry_pathname(ae));
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
	assertA(verify_data(buff, 1, DATA_SIZE));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test2.bin", archive_entry_pathname(ae));
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
	assertA(verify_data(buff, 2, DATA_SIZE));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test3.bin", archive_entry_pathname(ae));
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
	assertA(verify_data(buff, 3, DATA_SIZE));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test4.bin", archive_entry_pathname(ae));
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
	assertA(verify_data(buff, 4, DATA_SIZE));

	/* There should be no more files in this archive. */

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}