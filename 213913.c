action_save_thumb (ExifData *ed, ExifLog *log, ExifParams p, const char *fout)
{
	FILE *f;

	if (!ed) return;

	/* No thumbnail? Exit. */
	if (!ed->data) {
		exif_log (log, -1, "exif", _("'%s' does not "
			"contain a thumbnail!"), p.fin);
		return;
	}

	/* Save the thumbnail */
	f = fopen (fout, "wb");
	if (!f)
		exif_log (log, -1, "exif", _("Could not open '%s' for "
			"writing (%s)!"), fout, strerror (errno));
	else {
		if (fwrite (ed->data, 1, ed->size, f) != ed->size) {
			exif_log (log, -1, "exif", _("Could not write '%s' (%s)."),
				fout, strerror (errno));
		};
		if (fclose (f) == EOF)
			exif_log (log, -1, "exif", _("Could not write '%s' (%s)."),
				fout, strerror (errno));
		fprintf (stdout, _("Wrote file '%s'."), fout);
		fprintf (stdout, "\n");
	}
}