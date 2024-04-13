DEFINE_TEST(test_read_format_rar5_stored_skip_all_but_first)
{
	const char* fname = "test_read_format_rar5_stored_manyfiles.rar";
	char buf[405];

	/* Extract first, skip rest. */

	PROLOGUE(fname);
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
	assertA(405 == archive_read_data(a, buf, sizeof(buf)));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("cebula.txt", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}