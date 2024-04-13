DEFINE_TEST(test_read_format_rar5_compressed)
{
	const int DATA_SIZE = 1200;
	uint8_t buff[1200];

	PROLOGUE("test_read_format_rar5_compressed.rar");

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertA((int) archive_entry_mtime(ae) > 0);
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	verify_data(buff, 0, DATA_SIZE);

	EPILOGUE();
}