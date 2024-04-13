action_tag_list_machine (ExifData *ed, ExifParams p)
{
	if (!ed) return;

	if (p.ifd < EXIF_IFD_COUNT)
		/* Show only a single IFD */
		show_ifd_machine(ed->ifd[p.ifd], &p.use_ids);
	else
		/* Show contents of all IFDs */
		exif_data_foreach_content (ed, show_ifd_machine, &p.use_ids);

	if (ed->size)
		fprintf (stdout, _("ThumbnailSize\t%i\n"), ed->size);
}