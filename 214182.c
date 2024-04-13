DEFINE_TEST(test_read_format_rar5_block_by_block)
{
	/* This test uses strange buffer sizes intentionally. */

	struct archive_entry *ae;
	struct archive *a;
	uint8_t buf[173];
	int bytes_read;
	uint32_t computed_crc = 0;

	extract_reference_file("test_read_format_rar5_compressed.rar");
	assert((a = archive_read_new()) != NULL);
	assertA(0 == archive_read_support_filter_all(a));
	assertA(0 == archive_read_support_format_all(a));
	assertA(0 == archive_read_open_filename(a, "test_read_format_rar5_compressed.rar", 130));
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualString("test.bin", archive_entry_pathname(ae));
	assertEqualInt(1200, archive_entry_size(ae));

	/* File size is 1200 bytes, we're reading it using a buffer of 173 bytes.
	 * Libarchive is configured to use a buffer of 130 bytes. */

	while(1) {
		/* archive_read_data should return one of:
		 * a) 0, if there is no more data to be read,
		 * b) negative value, if there was an error,
		 * c) positive value, meaning how many bytes were read.
		 */

		bytes_read = archive_read_data(a, buf, sizeof(buf));
		assertA(bytes_read >= 0);
		if(bytes_read <= 0)
			break;

		computed_crc = crc32(computed_crc, buf, bytes_read);
	}

	assertEqualInt(computed_crc, 0x7CCA70CD);
	EPILOGUE();
}