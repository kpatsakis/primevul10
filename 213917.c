action_mnote_list (ExifData *ed, ExifParams p)
{
	char b[TAG_VALUE_BUF], b1[TAG_VALUE_BUF], b2[TAG_VALUE_BUF];
	unsigned int i, c, id;
	ExifMnoteData *n;
	const char *s;
	int fieldwidth, bytes;
	size_t width;

	n = exif_data_get_mnote_data (ed);
	if (!n) {
		printf (_("Unknown format or nonexistent MakerNote.\n"));
		return;
	}

	c = exif_mnote_data_count (n);
	if (!p.machine_readable) {
		switch (c) {
		case 0:
			printf (_("MakerNote does not contain any value.\n"));
			break;
		default:
			printf (ngettext("MakerNote contains %i value:\n",
					 "MakerNote contains %i values:\n",
					 c), c);
		}
	}
	for (i = 0; i < c; i++) {
	        if (p.use_ids) {
			id = exif_mnote_data_get_id  (n,i);
			sprintf(b1,"0x%04x",id);
		} else {
			s = C (exif_mnote_data_get_title (n, i));
			strncpy (b1, s && *s ? s : _("Unknown Tag"), TAG_VALUE_BUF);
			b1[sizeof(b1)-1] = 0;
		}
		if (p.machine_readable) {
			printf ("%s\t", b1);
		} else {
			fieldwidth = width = p.use_ids ? 6 : 20;
			bytes = exif_mbstrlen(b1, &width);
			printf ("%.*s%*s|", bytes, b1, fieldwidth-(int)width, "");
		}

		s = C (exif_mnote_data_get_value (n, i, b, TAG_VALUE_BUF));
		strncpy (b2, s ? s : _("Unknown value"), TAG_VALUE_BUF);
		b2[sizeof(b2)-1] = 0;
        	if (p.use_ids) {
			fputs (b2, stdout);
        	} else {
			fieldwidth = width = p.width-22;
			bytes = exif_mbstrlen(b2, &width);
			printf ("%.*s", bytes, b2);
		}
        	fputc ('\n', stdout);
	}
}