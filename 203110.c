theme_adium_dispose (GObject *object)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (object);

	if (priv->smiley_manager) {
		g_object_unref (priv->smiley_manager);
		priv->smiley_manager = NULL;
	}

	if (priv->last_contact) {
		g_object_unref (priv->last_contact);
		priv->last_contact = NULL;
	}

	if (priv->inspector_window) {
		gtk_widget_destroy (priv->inspector_window);
		priv->inspector_window = NULL;
	}

	if (priv->acked_messages.length > 0) {
		g_queue_clear (&priv->acked_messages);
	}

	G_OBJECT_CLASS (empathy_theme_adium_parent_class)->dispose (object);
}