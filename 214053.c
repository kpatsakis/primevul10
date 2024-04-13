DEFINE_TEST(test_read_format_rar5_blake2)
{
	const la_ssize_t proper_size = 814;
	uint8_t buf[814];

	PROLOGUE("test_read_format_rar5_blake2.rar");
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(proper_size, archive_entry_size(ae));

	/* Should blake2 calculation fail, we'll get a failure return
	 * value from archive_read_data(). */

	assertA(proper_size == archive_read_data(a, buf, proper_size));

	/* To be extra pedantic, let's also check crc32 of the poem. */
	assertEqualInt(crc32(0, buf, proper_size), 0x7E5EC49E);

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}