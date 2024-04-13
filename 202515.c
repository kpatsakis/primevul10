on_randr_event (GnomeRRScreen *screen, gpointer data)
{
        GsdXrandrManager *manager = GSD_XRANDR_MANAGER (data);
        GsdXrandrManagerPrivate *priv = manager->priv;
        guint32 change_timestamp, config_timestamp;

        if (!priv->running)
                return;

        gnome_rr_screen_get_timestamps (screen, &change_timestamp, &config_timestamp);

        if (change_timestamp > config_timestamp) {
                /* The event is due to an explicit configuration change.
                 *
                 * If the change was performed by us, then we need to do nothing.
                 *
                 * If the change was done by some other X client, we don't need
                 * to do anything, either; the screen is already configured.
                 */
                show_timestamps_dialog (manager, "ignoring since change > config");
        } else {
                /* Here, config_timestamp >= change_timestamp.  This means that
                 * the screen got reconfigured because of hotplug/unplug; the X
                 * server is just notifying us, and we need to configure the
                 * outputs in a sane way.
                 */
#if 1
                auto_configure_outputs (manager, config_timestamp);
#else
                /* WHY THIS CODE IS DISABLED:
                 *
                 * The strategy of "on hotplug or unsuspend, restore a
                 * known-good configuration, and fall back to autoconfiguration"
                 * works fine as long as you don't happen to be running
                 * gnome-display-properties and click its "Detect displays"
                 * button.
                 *
                 * If you do that, the RANDR calls from g-d-p will cause the X
                 * server to re-probe the RANDR outputs.  The server will send
                 * us an event, we'll restore the configuration to something
                 * else... and you'll be weirded out, because "just detecting
                 * your monitors" should not change the current configuration,
                 * right?
                 *
                 * We may need some kind of D-bus API so that g-d-p can inhibit
                 * this RANDR plugin's reconfiguration-fu when the "Detect
                 * displays" button is being used.
                 */

                char *intended_filename;
                GError *error;
                gboolean success;

                show_timestamps_dialog (manager, "need to deal with reconfiguration, as config >= change");

                intended_filename = gnome_rr_config_get_intended_filename ();

                error = NULL;
                success = apply_configuration_from_filename (manager, intended_filename, TRUE, config_timestamp, &error);
                g_free (intended_filename);

                if (!success) {
                        /* We don't bother checking the error type.
                         *
                         * Both G_FILE_ERROR_NOENT and
                         * GNOME_RR_ERROR_NO_MATCHING_CONFIG would mean, "there
                         * was no configuration to apply, or none that matched
                         * the current outputs", and in that case we need to run
                         * our fallback.
                         *
                         * Any other error means "we couldn't do the smart thing
                         * of using a previously- saved configuration, anyway,
                         * for some other reason.  In that case, we also need to
                         * run our fallback to avoid leaving the user with a
                         * bogus configuration.
                         */

                        if (error)
                                g_error_free (error);

                        auto_configure_outputs (manager, config_timestamp);
                }
#endif
        }

        refresh_tray_icon_menu_if_active (manager, MAX (change_timestamp, config_timestamp));
}