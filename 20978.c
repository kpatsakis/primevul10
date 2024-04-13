file_list_drag_begin (GtkWidget          *widget,
		      GdkDragContext     *context,
		      gpointer            data)
{
	FrWindow *window = data;

	debug (DEBUG_INFO, "::DragBegin -->\n");

	if (window->priv->activity_ref > 0)
		return FALSE;

	_g_clear_object (&window->priv->drag_destination_folder);

	g_free (window->priv->drag_base_dir);
	window->priv->drag_base_dir = NULL;

	gdk_property_change (gdk_drag_context_get_source_window (context),
			     XDS_ATOM, TEXT_ATOM,
			     8, GDK_PROP_MODE_REPLACE,
			     (guchar *) XDS_FILENAME,
			     strlen (XDS_FILENAME));

	return TRUE;
}