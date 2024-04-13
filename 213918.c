show_entry_xml (ExifEntry *e, void *data)
{
	unsigned char *ids = data;
	char v[TAG_VALUE_BUF], t[TAG_VALUE_BUF];

	if (*ids) {
		fprintf (stdout, "<x%04x>", e->tag);
		fprintf (stdout, "%s", escape_xml(exif_entry_get_value (e, v, sizeof (v))));
		fprintf (stdout, "</x%04x>", e->tag);
	} else {
		strncpy (t, exif_tag_get_title_in_ifd(e->tag, exif_entry_get_ifd(e)), sizeof (t));
		t[sizeof(t)-1] = 0;

		/* Remove invalid characters from tag eg. (, ), space */
		remove_bad_chars(t);

		fprintf (stdout, "\t<%s>", t);
		fprintf (stdout, "%s", escape_xml(exif_entry_get_value (e, v, sizeof (v))));
		fprintf (stdout, "</%s>\n", t);
	}
}