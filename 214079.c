DEFINE_TEST(test_read_format_rar5_stored_skip_all)
{
	const char* fname = "test_read_format_rar5_stored_manyfiles.rar";

	PROLOGUE(fname);
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("make_uue.tcl", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("cebula.txt", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}