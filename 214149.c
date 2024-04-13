DEFINE_TEST(test_read_format_rar5_symlink)
{
	const int DATA_SIZE = 5;
	uint8_t buff[5];

	PROLOGUE("test_read_format_rar5_symlink.rar");

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("file.txt", archive_entry_pathname(ae));
	assertEqualInt(AE_IFREG, archive_entry_filetype(ae));
	assertA((int) archive_entry_mtime(ae) > 0);
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("symlink.txt", archive_entry_pathname(ae));
	assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
	assertEqualString("file.txt", archive_entry_symlink(ae));
	assertEqualInt(AE_SYMLINK_TYPE_FILE, archive_entry_symlink_type(ae));
	assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("dirlink", archive_entry_pathname(ae));
	assertEqualInt(AE_IFLNK, archive_entry_filetype(ae));
	assertEqualString("dir", archive_entry_symlink(ae));
	assertEqualInt(AE_SYMLINK_TYPE_DIRECTORY, archive_entry_symlink_type(ae));
	assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("dir", archive_entry_pathname(ae));
	assertEqualInt(AE_IFDIR, archive_entry_filetype(ae));
	assertA(0 == archive_read_data(a, NULL, archive_entry_size(ae)));

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

	EPILOGUE();
}