fr_window_archive_reload (FrWindow *window)
{
	g_return_if_fail (window != NULL);

	if (window->priv->activity_ref > 0)
		return;
	if (window->priv->archive_new)
		return;
	if (window->archive == NULL)
		return;

	fr_window_archive_list (window);
}