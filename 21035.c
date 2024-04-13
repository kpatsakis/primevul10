fr_window_set_password (FrWindow   *window,
			const char *password)
{
	g_return_if_fail (window != NULL);

	if (window->priv->password == password)
		return;

	if (window->priv->password != NULL) {
		g_free (window->priv->password);
		window->priv->password = NULL;
	}

	if ((password != NULL) && (password[0] != '\0'))
		window->priv->password = g_strdup (password);
}