fr_window_set_dialog (FrWindow   *window,
		      const char *dialog_name,
		      GtkWidget  *dialog)
{
	g_object_set_data (G_OBJECT (dialog), DIALOG_NAME_KEY, (gpointer) _g_str_get_static (dialog_name));
	g_hash_table_insert (window->priv->named_dialogs, (gpointer) dialog_name, dialog);
	g_signal_connect (dialog,
			  "destroy",
			  G_CALLBACK (unset_dialog),
			  window);
}