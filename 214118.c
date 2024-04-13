DEFINE_TEST(test_read_format_rar5_multiarchive_skip_all)
{
	const char* reffiles[] = {
		"test_read_format_rar5_multiarchive.part01.rar",
		"test_read_format_rar5_multiarchive.part02.rar",
		"test_read_format_rar5_multiarchive.part03.rar",
		"test_read_format_rar5_multiarchive.part04.rar",
		"test_read_format_rar5_multiarchive.part05.rar",
		"test_read_format_rar5_multiarchive.part06.rar",
		"test_read_format_rar5_multiarchive.part07.rar",
		"test_read_format_rar5_multiarchive.part08.rar",
		NULL
	};

	PROLOGUE_MULTI(reffiles);
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("home/antek/temp/build/unrar5/libarchive/bin/bsdcat_test", archive_entry_pathname(ae));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("home/antek/temp/build/unrar5/libarchive/bin/bsdtar_test", archive_entry_pathname(ae));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}