DEFINE_TEST(test_read_format_rar5_multiarchive_skip_all_but_first)
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
	assertA(0 == extract_one(a, ae, 0x35277473));
	assertA(0 == archive_read_next_header(a, &ae));
	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}