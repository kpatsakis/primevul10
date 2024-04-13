show_entry (ExifEntry *entry, unsigned int machine_readable)
{
	ExifIfd ifd = exif_entry_get_ifd (entry);

	if (machine_readable) {
		char b[TAG_VALUE_BUF];

		fprintf (stdout, "%s\n", C(exif_entry_get_value (entry, b, sizeof (b))));
		return;
	}

	/*
	 * The C() macro can point to a static buffer so these printfs
	 * cannot be combined.
	 */
	printf (_("EXIF entry '%s' "),
		C(exif_tag_get_title_in_ifd (entry->tag, ifd)));
	printf (_("(0x%x, '%s') "),
		entry->tag,
		C(exif_tag_get_name_in_ifd (entry->tag, ifd)));
	printf (_("exists in IFD '%s':\n"),
		C(exif_ifd_get_name (ifd)));

	exif_entry_dump (entry, 0);
}