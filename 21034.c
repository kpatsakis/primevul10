fr_window_view_extraction_destination_folder (FrWindow *window)
{
	open_folder (GTK_WINDOW (window), window->priv->last_extraction_destination);
}