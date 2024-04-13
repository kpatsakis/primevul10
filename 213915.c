show_entry_machine (ExifEntry *e, void *data)
{
	unsigned int *ids = data;
	char v[TAG_VALUE_BUF];
	ExifIfd ifd = exif_entry_get_ifd (e);

	if (*ids) {
		fprintf (stdout, "0x%04x", e->tag);
	} else {
		fputs (CN (exif_tag_get_title_in_ifd (e->tag, ifd)), stdout);
	}
	fputc ('\t', stdout);
	fputs (CN (exif_entry_get_value (e, v, sizeof (v))), stdout);
	fputc ('\n', stdout);
}