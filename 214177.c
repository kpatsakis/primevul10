DEFINE_TEST(test_read_format_rar5_multiarchive_solid_skip_all)
{
	const char* reffiles[] = {
		"test_read_format_rar5_multiarchive_solid.part01.rar",
		"test_read_format_rar5_multiarchive_solid.part02.rar",
		"test_read_format_rar5_multiarchive_solid.part03.rar",
		"test_read_format_rar5_multiarchive_solid.part04.rar",
		NULL
	};

	PROLOGUE_MULTI(reffiles);
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("cebula.txt", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test1.bin", archive_entry_pathname(ae));
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
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("elf-Linux-ARMv7-ls", archive_entry_pathname(ae));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}