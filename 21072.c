get_icon (FrWindow  *window,
	  FileData  *fdata)
{
	GIcon     *icon = NULL;
	GdkPixbuf *pixbuf = NULL;

	if (fdata->link != NULL)
		icon = g_themed_icon_new ("emblem-symbolic-link");
	else {
		const char *content_type;

		if (file_data_is_dir (fdata))
			content_type = MIME_TYPE_DIRECTORY;
		else
			content_type = fdata->content_type;
		icon = g_content_type_get_icon (content_type);
	}

	pixbuf = gth_icon_cache_get_pixbuf (window->priv->list_icon_cache, icon);
	g_object_unref (icon);

	return pixbuf;
}