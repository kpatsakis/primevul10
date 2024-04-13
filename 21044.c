fr_archive_libarchive_get_capabilities (FrArchive  *archive,
					const char *mime_type,
					gboolean    check_command)
{
	FrArchiveCap capabilities;

	capabilities = FR_ARCHIVE_CAN_STORE_MANY_FILES;

	/* write-only formats */
	if (strcmp (mime_type, "application/x-7z-compressed") == 0) {
		capabilities |= FR_ARCHIVE_CAN_WRITE;
		return capabilities;
	}

	/* give priority to 7za that supports CAB files better. */
	if ((strcmp (mime_type, "application/vnd.ms-cab-compressed") == 0)
	    && _g_program_is_available ("7za", check_command))
	{
		return capabilities;
	}

	capabilities |= FR_ARCHIVE_CAN_READ;

	/* read-only formats */
	if ((strcmp (mime_type, "application/vnd.ms-cab-compressed") == 0)
	    || (strcmp (mime_type, "application/x-lha") == 0)
	    || (strcmp (mime_type, "application/x-xar") == 0))
	{
		return capabilities;
	}

	/* all other formats can be read and written */
	capabilities |= FR_ARCHIVE_CAN_WRITE;

	return capabilities;
}