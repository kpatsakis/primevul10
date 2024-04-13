action_show_tag (ExifData *ed, ExifLog *log, ExifParams p)
{
	ExifEntry *e;
	unsigned int i;

	if (!ed) return;

	/* We have one optional parameter: ifd */
	if ((p.ifd >= EXIF_IFD_0) && (p.ifd < EXIF_IFD_COUNT)) {
		if ((e = exif_content_get_entry (ed->ifd[p.ifd], p.tag)))
			show_entry (e, p.machine_readable);
		else
			exif_log (log, -1, "exif", _("IFD '%s' "
				"does not contain tag '%s'."),
					exif_ifd_get_name (p.ifd),
					exif_tag_get_name (p.tag));
	} else {
		if (!exif_data_get_entry (ed, p.tag))
			exif_log (log, -1, "exif", _("'%s' does not contain "
				"tag '%s'."), p.fin,
				exif_tag_get_name (p.tag));
		else for (i = 0; i < EXIF_IFD_COUNT; i++)
			if ((e = exif_content_get_entry (ed->ifd[i], p.tag)))
				show_entry (e, p.machine_readable);
	}
}