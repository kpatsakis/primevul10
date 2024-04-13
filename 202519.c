handle_fn_f7 (GsdXrandrManager *mgr, guint32 timestamp)
{
        GsdXrandrManagerPrivate *priv = mgr->priv;
        GnomeRRScreen *screen = priv->rw_screen;
        GnomeRRConfig *current;
        GError *error;

        /* Theory of fn-F7 operation
         *
         * We maintain a datastructure "fn_f7_status", that contains
         * a list of GnomeRRConfig's. Each of the GnomeRRConfigs has a
         * mode (or "off") for each connected output.
         *
         * When the user hits fn-F7, we cycle to the next GnomeRRConfig
         * in the data structure. If the data structure does not exist, it
         * is generated. If the configs in the data structure do not match
         * the current hardware reality, it is regenerated.
         *
         */
        g_debug ("Handling fn-f7");

        error = NULL;
        if (!gnome_rr_screen_refresh (screen, &error) && error) {
                char *str;

                str = g_strdup_printf (_("Could not refresh the screen information: %s"), error->message);
                g_error_free (error);

                error_message (mgr, str, NULL, _("Trying to switch the monitor configuration anyway."));
                g_free (str);
        }

        if (!priv->fn_f7_configs)
                generate_fn_f7_configs (mgr);

        current = gnome_rr_config_new_current (screen);

        if (priv->fn_f7_configs &&
            (!gnome_rr_config_match (current, priv->fn_f7_configs[0]) ||
             !gnome_rr_config_equal (current, priv->fn_f7_configs[mgr->priv->current_fn_f7_config]))) {
                    /* Our view of the world is incorrect, so regenerate the
                     * configurations
                     */
                    generate_fn_f7_configs (mgr);
            }

        gnome_rr_config_free (current);

        if (priv->fn_f7_configs) {
                mgr->priv->current_fn_f7_config++;

                if (priv->fn_f7_configs[mgr->priv->current_fn_f7_config] == NULL)
                        mgr->priv->current_fn_f7_config = 0;

                g_debug ("cycling to next configuration (%d)", mgr->priv->current_fn_f7_config);

                print_configuration (priv->fn_f7_configs[mgr->priv->current_fn_f7_config], "new config");

                g_debug ("applying");

                error = NULL;
                if (!gnome_rr_config_apply_with_time (priv->fn_f7_configs[mgr->priv->current_fn_f7_config], screen, timestamp, &error)) {
                        error_message (mgr, _("Could not switch the monitor configuration"), error, NULL);
                        g_error_free (error);
                }
        }
        else {
                g_debug ("no configurations generated");
        }
        g_debug ("done handling fn-f7");
}