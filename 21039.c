fr_window_open_files_with_application (FrWindow *window,
				       GList    *file_list,
				       GAppInfo *app)
{
	GList               *uris;
	GList               *scan;
	GdkAppLaunchContext *context;
	GError              *error = NULL;

	if (window->priv->activity_ref > 0)
		return;

	uris = NULL;
	for (scan = file_list; scan; scan = scan->next)
		uris = g_list_prepend (uris, g_file_get_uri (G_FILE (scan->data)));

	context = gdk_display_get_app_launch_context (gtk_widget_get_display (GTK_WIDGET (window)));
	gdk_app_launch_context_set_screen (context, gtk_widget_get_screen (GTK_WIDGET (window)));
	gdk_app_launch_context_set_timestamp (context, 0);

	if (! g_app_info_launch_uris (app, uris, G_APP_LAUNCH_CONTEXT (context), &error)) {
		_gtk_error_dialog_run (GTK_WINDOW (window),
				       _("Could not perform the operation"),
				       "%s",
				       error->message);
		g_clear_error (&error);
	}

	g_object_unref (context);
	_g_string_list_free (uris);
}