DEFINE_TEST(test_read_format_rar5_stored_skip_in_part)
{
	const char* fname = "test_read_format_rar5_stored_manyfiles.rar";
	char buf[6];

	/* Skip first, extract in part rest. */

	PROLOGUE(fname);
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("cebula.txt", archive_entry_pathname(ae));
	assertA(6 == archive_read_data(a, buf, 6));
	assertEqualInt(0, memcmp(buf, "Cebula", 6));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertA(4 == archive_read_data(a, buf, 4));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}