fr_archive_stoppable_cb (FrArchive *archive,
			 gboolean   stoppable,
			 FrWindow  *window)
{
	window->priv->stoppable = stoppable;
	if (window->priv->progress_dialog != NULL)
		gtk_dialog_set_response_sensitive (GTK_DIALOG (window->priv->progress_dialog),
						   GTK_RESPONSE_OK,
						   stoppable);
	return TRUE;
}