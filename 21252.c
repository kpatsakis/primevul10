gdk_pixbuf__xbm_image_load_increment (gpointer       data,
                                      const guchar  *buf,
                                      guint          size,
                                      GError       **error)
{
	XBMData *context = (XBMData *) data;

	g_return_val_if_fail (data != NULL, FALSE);

	if (fwrite (buf, sizeof (guchar), size, context->file) != size) {
		gint save_errno = errno;
		context->all_okay = FALSE;
                g_set_error_literal (error,
                                     G_FILE_ERROR,
                                     g_file_error_from_errno (save_errno),
                                     _("Failed to write to temporary file when loading XBM image"));
		return FALSE;
	}

	return TRUE;
}