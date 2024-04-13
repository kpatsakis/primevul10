progress_dialog_update_action_description (FrWindow *window)
{
	GFile *current_archive;
	char  *description;


	if (window->priv->progress_dialog == NULL)
		return;

	if (window->priv->saving_file != NULL)
		current_archive = window->priv->saving_file;
	else if (window->priv->working_archive != NULL)
		current_archive = window->priv->working_archive;
	else
		current_archive = window->priv->archive_file;

	_g_clear_object (&window->priv->pd_last_archive);
	if (current_archive != NULL)
		window->priv->pd_last_archive = g_object_ref (current_archive);

	description = get_action_description (window, window->priv->action, window->priv->pd_last_archive);
	progress_dialog_set_action_description (window, description);

	g_free (description);
}