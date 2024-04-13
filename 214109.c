DEFINE_TEST(test_read_format_rar5_extract_win32)
{
	PROLOGUE("test_read_format_rar5_win32.rar");
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("testdir", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFDIR | 0755);
	assertA(0 == extract_one(a, ae, 0));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
	assertA(0 == extract_one(a, ae, 0x7CCA70CD));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test1.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
	assertA(0 == extract_one(a, ae, 0x7E13B2C6));
	assertA(0 == archive_read_next_header(a, &ae));
	/* Read only file */
	assertEqualString("test2.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0444);
	assertA(0 == extract_one(a, ae, 0xF166AFCB));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test3.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
	assertA(0 == extract_one(a, ae, 0x9FB123D9));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test4.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
	assertA(0 == extract_one(a, ae, 0x10C43ED4));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test5.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
	assertA(0 == extract_one(a, ae, 0xB9D155F2));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test6.bin", archive_entry_pathname(ae));
	assertEqualInt(archive_entry_mode(ae), AE_IFREG | 0644);
	assertA(0 == extract_one(a, ae, 0x36A448FF));
	EPILOGUE();
}