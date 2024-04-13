DEFINE_TEST(test_read_format_rar5_fileattr)
{
	unsigned long set, clear, flag;

	flag = 0;

	PROLOGUE("test_read_format_rar5_fileattr.rar");

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0444 | AE_IFREG);
	assertEqualString("readonly.txt", archive_entry_pathname(ae));
	assertEqualString("rdonly", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_READONLY;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_READONLY;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0644 | AE_IFREG);
	assertEqualString("hidden.txt", archive_entry_pathname(ae));
	assertEqualString("hidden", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_HIDDEN;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0644 | AE_IFREG);
	assertEqualString("system.txt", archive_entry_pathname(ae));
	assertEqualString("system", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_SYSTEM;;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_SYSTEM;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0444 | AE_IFREG);
	assertEqualString("ro_hidden.txt", archive_entry_pathname(ae));
	assertEqualString("rdonly,hidden", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_READONLY | UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0555 | AE_IFDIR);
	assertEqualString("dir_readonly", archive_entry_pathname(ae));
	assertEqualString("rdonly", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_READONLY;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_READONLY;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0755 | AE_IFDIR);
	assertEqualString("dir_hidden", archive_entry_pathname(ae));
	assertEqualString("hidden", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_HIDDEN;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0755 | AE_IFDIR);
	assertEqualString("dir_system", archive_entry_pathname(ae));
	assertEqualString("system", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_SYSTEM;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_SYSTEM;
#endif
	assertEqualInt(flag, set & flag);

	assertA(0 == archive_read_next_header(a, &ae));
	assertEqualInt(archive_entry_mode(ae), 0555 | AE_IFDIR);
	assertEqualString("dir_rohidden", archive_entry_pathname(ae));
	assertEqualString("rdonly,hidden", archive_entry_fflags_text(ae));
	archive_entry_fflags(ae, &set, &clear);
#if defined(__FreeBSD__)
	flag = UF_READONLY | UF_HIDDEN;
#elif defined(_WIN32) && !defined(CYGWIN)
	flag = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN;
#endif
	assertEqualInt(flag, set & flag);

	EPILOGUE();
}