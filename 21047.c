fr_window_deactivate_filter (FrWindow *window)
{
	GtkAction *action;

	action = gtk_action_group_get_action (window->priv->actions, "Find");
	g_object_set (action, "active", FALSE, NULL);
}