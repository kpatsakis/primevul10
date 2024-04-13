gdk_pixbuf__xbm_image_stop_load (gpointer   data,
                                 GError   **error)
{
	XBMData *context = (XBMData*) data;
        gboolean retval = TRUE;

	g_return_val_if_fail (data != NULL, TRUE);

	fflush (context->file);
	rewind (context->file);
	if (context->all_okay) {
                GdkPixbuf *pixbuf;
                pixbuf = gdk_pixbuf__xbm_image_load_real (context->file, 
							  context,
                                                          error);
                if (pixbuf == NULL)
                        retval = FALSE;
		else
			g_object_unref (pixbuf);
        }

	fclose (context->file);
	g_unlink (context->tempname);
	g_free (context->tempname);
	g_free ((XBMData *) context);

        return retval;
}