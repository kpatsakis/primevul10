DEFINE_TEST(test_read_format_rar5_solid_skip_all_but_second)
{
	const char* reffile = "test_read_format_rar5_solid.rar";

	/* Skip first, extract second, skip rest */

	PROLOGUE(reffile);
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test1.bin", archive_entry_pathname(ae));
	assertA(0 == extract_one(a, ae, 0x7E13B2C6));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test2.bin", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test3.bin", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test4.bin", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test5.bin", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test6.bin", archive_entry_pathname(ae));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}