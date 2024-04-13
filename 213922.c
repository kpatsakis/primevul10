action_remove_tag (ExifData *ed, ExifLog *log, ExifParams p)
{
	ExifIfd ifd;
	ExifEntry *e;

	/* We do have 2 optional parameters: ifd and tag */
	if (p.tag == EXIF_INVALID_TAG && (p.ifd < EXIF_IFD_0 || p.ifd >= EXIF_IFD_COUNT))
		for (ifd = EXIF_IFD_0; ifd < EXIF_IFD_COUNT; ifd++)
			while (ed->ifd[ifd] && ed->ifd[ifd]->count)
				exif_content_remove_entry (ed->ifd[ifd],
					ed->ifd[ifd]->entries[0]);
	else if (p.tag == EXIF_INVALID_TAG)
		while (ed->ifd[p.ifd] && ed->ifd[p.ifd]->count)
			exif_content_remove_entry (ed->ifd[p.ifd],
				ed->ifd[p.ifd]->entries[0]);
	else if (p.ifd < EXIF_IFD_0 || p.ifd >= EXIF_IFD_COUNT)
		while ((e = exif_data_get_entry (ed, p.tag)))
			exif_content_remove_entry (e->parent, e);
	else if (!((e = exif_content_get_entry (ed->ifd[p.ifd], p.tag))))
		exif_log (log, -1, "exif", _("IFD '%s' does not contain a "
			"tag '%s'!"), exif_ifd_get_name (p.ifd),
			exif_tag_get_name_in_ifd (p.tag, p.ifd));
	else
		exif_content_remove_entry (ed->ifd[p.ifd], e);
}