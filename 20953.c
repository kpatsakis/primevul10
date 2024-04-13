fr_window_set_password_for_second_archive (FrWindow   *window,
					   const char *password)
{
	g_return_if_fail (window != NULL);

	if (window->priv->second_password != NULL) {
		g_free (window->priv->second_password);
		window->priv->second_password = NULL;
	}

	if ((password != NULL) && (password[0] != '\0'))
		window->priv->second_password = g_strdup (password);
}