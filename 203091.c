theme_adium_finalize (GObject *object)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (object);

	empathy_adium_data_unref (priv->data);

	g_object_unref (priv->gsettings_chat);
	g_object_unref (priv->gsettings_desktop);

	G_OBJECT_CLASS (empathy_theme_adium_parent_class)->finalize (object);
}