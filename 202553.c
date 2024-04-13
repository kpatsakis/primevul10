output_rotation_item_activate_cb (GtkCheckMenuItem *item, gpointer data)
{
        GsdXrandrManager *manager = GSD_XRANDR_MANAGER (data);
        struct GsdXrandrManagerPrivate *priv = manager->priv;
        GnomeOutputInfo *output;
        GnomeRRRotation rotation;
        GError *error;

	/* Not interested in deselected items */
	if (!gtk_check_menu_item_get_active (item))
		return;

        ensure_current_configuration_is_saved ();

        output = g_object_get_data (G_OBJECT (item), "output");
        rotation = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (item), "rotation"));

        output->rotation = rotation;

        error = NULL;
        if (!gnome_rr_config_save (priv->configuration, &error)) {
                error_message (manager, _("Could not save monitor configuration"), error, NULL);
                if (error)
                        g_error_free (error);

                return;
        }

        try_to_apply_intended_configuration (manager, NULL, gtk_get_current_event_time (), NULL); /* NULL-GError */
}