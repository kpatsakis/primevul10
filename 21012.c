get_emblem (FrWindow *window,
	    FileData *fdata)
{
	const char *emblem_name;
	GIcon      *icon;
	GdkPixbuf  *pixbuf;

	emblem_name = NULL;
	if (fdata->encrypted)
		emblem_name = "emblem-nowrite";

	if (emblem_name == NULL)
		return NULL;

	icon = g_themed_icon_new (emblem_name);
	pixbuf = gth_icon_cache_get_pixbuf (window->priv->list_icon_cache, icon);
	g_object_unref (icon);

	return pixbuf;
}