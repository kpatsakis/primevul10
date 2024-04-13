DEFINE_TEST(test_read_format_rar5_arm_filter)
{
	/* This test unpacks a file that uses an ARM filter. The DELTA
	 * and X86 filters are tested implicitly in the "multiarchive_skip"
	 * test. */

	const la_ssize_t proper_size = 90808;
	uint8_t buf[90808];

	PROLOGUE("test_read_format_rar5_arm.rar");
	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(proper_size, archive_entry_size(ae));
	assertA(proper_size == archive_read_data(a, buf, proper_size));

	/* Yes, RARv5 unpacker itself should calculate the CRC, but in case
	 * the DONT_FAIL_ON_CRC_ERROR define option is enabled during compilation,
	 * let's still fail the test if the unpacked data is wrong. */
	assertEqualInt(crc32(0, buf, proper_size), 0x886F91EB);

	assertA(ARCHIVE_EOF == archive_read_next_header(a, &ae));
	EPILOGUE();
}