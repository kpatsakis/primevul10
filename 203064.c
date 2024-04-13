theme_adium_load_template (EmpathyThemeAdium *theme)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	gchar                 *basedir_uri;
	gchar                 *variant_path;
	gchar                 *template;

	priv->pages_loading++;
	basedir_uri = g_strconcat ("file://", priv->data->basedir, NULL);
	variant_path = adium_info_dup_path_for_variant (priv->data->info,
		priv->variant);
	template = string_with_format (priv->data->template_html,
		variant_path, NULL);
	webkit_web_view_load_html_string (WEBKIT_WEB_VIEW (theme),
					  template, basedir_uri);
	g_free (basedir_uri);
	g_free (variant_path);
	g_free (template);
}