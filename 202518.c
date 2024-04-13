auto_configure_outputs (GsdXrandrManager *manager, guint32 timestamp)
{
        GsdXrandrManagerPrivate *priv = manager->priv;
        GnomeRRConfig *config;
        int i;
        GList *just_turned_on;
        GList *l;
        int x;
        GError *error;

        config = gnome_rr_config_new_current (priv->rw_screen);

        /* For outputs that are connected and on (i.e. they have a CRTC assigned
         * to them, so they are getting a signal), we leave them as they are
         * with their current modes.
         *
         * For other outputs, we will turn on connected-but-off outputs and turn
         * off disconnected-but-on outputs.
         *
         * FIXME: If an output remained connected+on, it would be nice to ensure
         * that the output's CRTCs still has a reasonable mode (think of
         * changing one monitor for another with different capabilities).
         */

        just_turned_on = NULL;

        for (i = 0; config->outputs[i] != NULL; i++) {
                GnomeOutputInfo *output = config->outputs[i];

                if (output->connected && !output->on) {
                        output->on = TRUE;
                        output->rotation = GNOME_RR_ROTATION_0;
                        just_turned_on = g_list_prepend (just_turned_on, GINT_TO_POINTER (i));
                } else if (!output->connected && output->on)
                        output->on = FALSE;
        }

        /* Now, lay out the outputs from left to right.  Put first the outputs
         * which remained on; put last the outputs that were newly turned on.
         */

        x = 0;

        /* First, outputs that remained on */

        for (i = 0; config->outputs[i] != NULL; i++) {
                GnomeOutputInfo *output = config->outputs[i];

                if (g_list_find (just_turned_on, GINT_TO_POINTER (i)))
                        continue;

                if (output->on) {
                        g_assert (output->connected);

                        output->x = x;
                        output->y = 0;

                        x += output->width;
                }
        }

        /* Second, outputs that were newly-turned on */

        for (l = just_turned_on; l; l = l->next) {
                GnomeOutputInfo *output;

                i = GPOINTER_TO_INT (l->data);
                output = config->outputs[i];

                g_assert (output->on && output->connected);

                output->x = x;
                output->y = 0;

                /* since the output was off, use its preferred width/height (it doesn't have a real width/height yet) */
                output->width = output->pref_width;
                output->height = output->pref_height;

                x += output->width;
        }

        /* Apply the configuration! */

        error = NULL;
        if (!gnome_rr_config_apply_with_time (config, priv->rw_screen, timestamp, &error)) {
                error_message (manager, _("Could not switch the monitor configuration"), error, NULL);
                g_error_free (error);
        }

        g_list_free (just_turned_on);
        gnome_rr_config_free (config);

        /* Finally, even though we did a best-effort job in sanitizing the
         * outputs, we don't know the physical layout of the monitors.  We'll
         * start the display capplet so that the user can tweak things to his
         * liking.
         */
}