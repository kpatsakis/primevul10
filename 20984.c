get_mime_type_icon (FrWindow   *window,
		    const char *mime_type)
{
	GIcon     *icon;
	GdkPixbuf *pixbuf;

	icon = g_content_type_get_icon (mime_type);
	pixbuf = gth_icon_cache_get_pixbuf (window->priv->tree_icon_cache, icon);

	g_object_unref (icon);

	return pixbuf;
}