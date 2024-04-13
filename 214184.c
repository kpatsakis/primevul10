DEFINE_TEST(test_read_format_rar5_owner)
{
	const int DATA_SIZE = 5;
	uint8_t buff[5];

	PROLOGUE("test_read_format_rar5_owner.rar");

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("root.txt", archive_entry_pathname(ae));
	assertEqualString("root", archive_entry_uname(ae));
	assertEqualString("wheel", archive_entry_gname(ae));
	assertA((int) archive_entry_mtime(ae) > 0);
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("nobody.txt", archive_entry_pathname(ae));
	assertEqualString("nobody", archive_entry_uname(ae));
	assertEqualString("nogroup", archive_entry_gname(ae));
	assertA((int) archive_entry_mtime(ae) > 0);
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("numeric.txt", archive_entry_pathname(ae));
	assertEqualInt(9999, archive_entry_uid(ae));
	assertEqualInt(8888, archive_entry_gid(ae));
	assertA((int) archive_entry_mtime(ae) > 0);
	assertEqualInt(DATA_SIZE, archive_entry_size(ae));
	assertA(DATA_SIZE == archive_read_data(a, buff, DATA_SIZE));

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

	EPILOGUE();
}