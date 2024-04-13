DEFINE_TEST(test_read_format_rar5_extra_field_version)
{
	PROLOGUE("test_read_format_rar5_extra_field_version.rar");

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("bin/2to3;1", archive_entry_pathname(ae));
	assertA(0 == extract_one(a, ae, 0xF24181B7));

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("bin/2to3", archive_entry_pathname(ae));
	assertA(0 == extract_one(a, ae, 0xF24181B7));

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));

	EPILOGUE();
}