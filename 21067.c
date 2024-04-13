fr_window_get_password_for_second_archive (FrWindow *window)
{
	g_return_val_if_fail (window != NULL, NULL);

	return window->priv->second_password;
}