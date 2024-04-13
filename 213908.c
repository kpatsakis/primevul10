action_tag_list (ExifData *ed, ExifParams p)
{
	ExifByteOrder order;
	const char *s;
	int fieldwidth, bytes;
	size_t width;

	if (!ed)
		return;

	order = exif_data_get_byte_order (ed);
	printf (_("EXIF tags in '%s' ('%s' byte order):"), p.fin,
		exif_byte_order_get_name (order));
	fputc ('\n', stdout);
	print_hline (p.use_ids, p.width);

	fieldwidth = width = p.use_ids ? 6 : 20;
	s = _("Tag");
	bytes = exif_mbstrlen(s, &width);
	printf ("%.*s%*s", bytes, s, fieldwidth-(int)width, "");
	fputc ('|', stdout);

	fieldwidth = width = p.use_ids ? p.width-8 : p.width-22;
	s = _("Value");
	bytes = exif_mbstrlen(s, &width);
	printf ("%.*s", bytes, s);
        fputc ('\n', stdout);
        print_hline (p.use_ids, p.width);

	if (p.ifd < EXIF_IFD_COUNT)
		/* Show only a single IFD */
		show_ifd(ed->ifd[p.ifd], &p);
	else
		/* Show contents of all IFDs */
		exif_data_foreach_content (ed, show_ifd, &p);

        print_hline (p.use_ids, p.width);
        if (ed->size) {
                printf (_("EXIF data contains a thumbnail "
			  "(%i bytes)."), ed->size);
                fputc ('\n', stdout);
        }
}