empathy_theme_adium_set_variant (EmpathyThemeAdium *theme,
				 const gchar *variant)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	gchar *variant_path;
	gchar *script;

	if (!tp_strdiff (priv->variant, variant)) {
		return;
	}

	g_free (priv->variant);
	priv->variant = g_strdup (variant);

	if (priv->in_construction) {
		return;
	}

	DEBUG ("Update view with variant: '%s'", variant);
	variant_path = adium_info_dup_path_for_variant (priv->data->info,
		priv->variant);
	script = g_strdup_printf ("setStylesheet(\"mainStyle\",\"%s\");", variant_path);

	webkit_web_view_execute_script (WEBKIT_WEB_VIEW (theme), script);

	g_free (variant_path);
	g_free (script);

	g_object_notify (G_OBJECT (theme), "variant");
}