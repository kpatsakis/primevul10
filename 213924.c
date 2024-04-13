action_save (ExifData *ed, ExifLog *log, ExifParams p, const char *fout)
{
	JPEGData *jdata;
	unsigned char *d = NULL;
	unsigned int ds;

	/* Parse the JPEG file. */
	jdata = jpeg_data_new ();
	jpeg_data_log (jdata, log);
	jpeg_data_load_file (jdata, p.fin);

	/* Make sure the EXIF data is not too big. */
	exif_data_save_data (ed, &d, &ds);
	if (ds) {
		free (d);
		if (ds > 0xffff)
			exif_log (log, -1, "exif", _("Too much EXIF data "
				"(%i bytes). Only %i bytes are allowed."),
				ds, 0xffff);
	};

	jpeg_data_set_exif_data (jdata, ed);

	/* Save the modified image. */
	if (jpeg_data_save_file (jdata, fout) == 0)
			exif_log (log, -1, "exif", _("Could not write "
				"'%s' (%s)."), fout, strerror (errno));
	jpeg_data_unref (jdata);

	fprintf (stdout, _("Wrote file '%s'."), fout);
	fprintf (stdout, "\n");
}