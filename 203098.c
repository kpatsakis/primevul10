theme_adium_inspect_web_view_cb (WebKitWebInspector *inspector,
				 WebKitWebView      *web_view,
				 EmpathyThemeAdium  *theme)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	GtkWidget             *scrolled_window;
	GtkWidget             *inspector_web_view;

	if (!priv->inspector_window) {
		/* Create main window */
		priv->inspector_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_default_size (GTK_WINDOW (priv->inspector_window),
					     800, 600);
		g_signal_connect (priv->inspector_window, "delete-event",
				  G_CALLBACK (gtk_widget_hide_on_delete), NULL);

		/* Pack a scrolled window */
		scrolled_window = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
		gtk_container_add (GTK_CONTAINER (priv->inspector_window),
				   scrolled_window);
		gtk_widget_show  (scrolled_window);

		/* Pack a webview in the scrolled window. That webview will be
		 * used to render the inspector tool.  */
		inspector_web_view = webkit_web_view_new ();
		gtk_container_add (GTK_CONTAINER (scrolled_window),
				   inspector_web_view);
		gtk_widget_show (scrolled_window);

		return WEBKIT_WEB_VIEW (inspector_web_view);
	}

	return NULL;
}