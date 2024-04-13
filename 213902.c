action_tag_table (ExifData *ed, ExifParams p)
{
	unsigned int tag;
	const char *name;
	char txt[TAG_VALUE_BUF];
	ExifIfd i;
	int fieldwidth, bytes;
	size_t width;

#define ENTRY_FOUND     "   *   "
#define ENTRY_NOT_FOUND "   -   "

	snprintf (txt, sizeof (txt) - 1, _("EXIF tags in '%s':"), p.fin);
	fieldwidth = width = p.width - 36;
	bytes = exif_mbstrlen(txt, &width);
	printf ("%.*s%*s", bytes, txt, fieldwidth-(int)width, "");

	for (i = (ExifIfd)0; i < EXIF_IFD_COUNT; i++) {
		int space;
		fieldwidth = width = 7;
		bytes = exif_mbstrlen(exif_ifd_get_name (i), &width);
		space = fieldwidth-width;
		printf ("%*s%.*s%*s", space/2, "", bytes, exif_ifd_get_name (i),
			space - space/2, "");
	}
	fputc ('\n', stdout);

	for (tag = 0; tag < 0xffff; tag++) {
		/*
		 * Display the name of the first tag of this number found.
		 * Since there is some overlap (e.g. with GPS tags), this
		 * name could sometimes be incorrect for the specific tags
		 * found in this file.
		 */
		name = exif_tag_get_title(tag);
		if (!name)
			continue;

		fieldwidth = width = p.width - 43;
		bytes = exif_mbstrlen(C(name), &width);
		printf ("0x%04x %.*s%*s",
			tag, bytes, C(name), fieldwidth-(int)width, "");
		for (i = (ExifIfd)0; i < EXIF_IFD_COUNT; i++)
			if (exif_content_get_entry (ed->ifd[i], tag))
				printf (ENTRY_FOUND);
			else
				printf (ENTRY_NOT_FOUND);
		fputc ('\n', stdout);
	}
}