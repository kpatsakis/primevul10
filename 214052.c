DEFINE_TEST(test_read_format_rar5_stored)
{
	const char helloworld_txt[] = "hello libarchive test suite!\n";
	la_ssize_t file_size = sizeof(helloworld_txt) - 1;
	char buff[64];

	PROLOGUE("test_read_format_rar5_stored.rar");

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("helloworld.txt", archive_entry_pathname(ae));
	assertA((int) archive_entry_mtime(ae) > 0);
	assertA((int) archive_entry_ctime(ae) == 0);
	assertA((int) archive_entry_atime(ae) == 0);
	assertEqualInt(file_size, archive_entry_size(ae));
	assertEqualInt(33188, archive_entry_mode(ae));
	assertA(file_size == archive_read_data(a, buff, file_size));
	assertEqualMem(buff, helloworld_txt, file_size);
	assertEqualInt(archive_entry_is_encrypted(ae), 0);

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

	EPILOGUE();
}