action_insert_thumb (ExifData *ed, ExifLog *log, ExifParams p)
{
	FILE *f;

	if (!ed) return;

	/* Get rid of the thumbnail */
	action_remove_thumb (ed, log, p);

	/* Insert new thumbnail */
	f = fopen (p.set_thumb, "rb");
	if (!f) {
		exif_log (log, -1, "exif", _("Could not open "
			"'%s' (%s)!"), p.set_thumb, strerror (errno));
	} else {
		long fsize;
		if (fseek (f, 0, SEEK_END) < 0) {
			fclose(f);
			exif_log (log, -1, "exif", _("Could not determine size of "
				"'%s' (%s)."), p.set_thumb, strerror (errno));
			return;
		}
		fsize = ftell (f);
		if (fsize < 0) {
			fclose(f);
			exif_log (log, -1, "exif", _("Could not determine size of "
				"'%s' (%s)."), p.set_thumb, strerror (errno));
			return;
		}
		ed->size = fsize;
		ed->data = malloc (sizeof (char) * ed->size);
		if (ed->size && !ed->data) {
			EXIF_LOG_NO_MEMORY (log, "exif", sizeof (char) * ed->size);
			exit (1);
		}
		if (fseek (f, 0, SEEK_SET) < 0) {
			fclose(f);
			exif_log (log, -1, "exif", _("Could not determine size of "
				"'%s' (%s)."), p.set_thumb, strerror (errno));
			return;
		}
		if (fread (ed->data, sizeof (char), ed->size, f) != ed->size)
			exif_log (log, -1, "exif", _("Could not read "
				"'%s' (%s)."), p.set_thumb, strerror (errno));
		if (fclose (f) < 0)
			exif_log (log, -1, "exif", _("Could not read "
				"'%s' (%s)."), p.set_thumb, strerror (errno));
	}
}