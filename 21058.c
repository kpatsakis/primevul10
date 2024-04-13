unset_dialog (GtkWidget *object,
              gpointer   user_data)
{
	FrWindow   *window = user_data;
	const char *dialog_name;

	dialog_name = g_object_get_data (G_OBJECT (object), DIALOG_NAME_KEY);
	if (dialog_name != NULL)
		g_hash_table_remove (window->priv->named_dialogs, dialog_name);
}