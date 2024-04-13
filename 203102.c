theme_adium_inspector_close_window_cb (WebKitWebInspector *inspector,
				       EmpathyThemeAdium  *theme)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);

	if (priv->inspector_window) {
		gtk_widget_hide (priv->inspector_window);
	}

	return TRUE;
}