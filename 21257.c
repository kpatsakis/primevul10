gdk_pixbuf__xbm_image_begin_load (GdkPixbufModuleSizeFunc       size_func,
                                  GdkPixbufModulePreparedFunc   prepare_func,
				  GdkPixbufModuleUpdatedFunc    update_func,
				  gpointer                      user_data,
				  GError                      **error)
{
	XBMData *context;
	gint fd;

	context = g_new (XBMData, 1);
	context->prepare_func = prepare_func;
	context->update_func = update_func;
	context->user_data = user_data;
	context->all_okay = TRUE;
	fd = g_file_open_tmp ("gdkpixbuf-xbm-tmp.XXXXXX",
			      &context->tempname,
			      NULL);
	if (fd < 0) {
		g_free (context);
		return NULL;
	}

	context->file = fdopen (fd, "w+");
	if (context->file == NULL) {
		g_free (context->tempname);
		g_free (context);
		return NULL;
	}

	return context;
}