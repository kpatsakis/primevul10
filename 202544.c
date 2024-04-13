gsd_xrandr_manager_start (GsdXrandrManager *manager,
                          GError          **error)
{
        g_debug ("Starting xrandr manager");
        gnome_settings_profile_start (NULL);

        manager->priv->rw_screen = gnome_rr_screen_new (
                gdk_screen_get_default (), on_randr_event, manager, error);

        if (manager->priv->rw_screen == NULL)
                return FALSE;

        manager->priv->running = TRUE;
        manager->priv->client = gconf_client_get_default ();

        g_assert (manager->priv->notify_id == 0);

        gconf_client_add_dir (manager->priv->client, CONF_DIR,
                              GCONF_CLIENT_PRELOAD_ONELEVEL,
                              NULL);

        manager->priv->notify_id =
                gconf_client_notify_add (
                        manager->priv->client, CONF_DIR,
                        (GConfClientNotifyFunc)on_config_changed,
                        manager, NULL, NULL);

        if (manager->priv->keycode) {
                gdk_error_trap_push ();

                XGrabKey (gdk_x11_get_default_xdisplay(),
                          manager->priv->keycode, AnyModifier,
                          gdk_x11_get_default_root_xwindow(),
                          True, GrabModeAsync, GrabModeAsync);

                gdk_flush ();
                gdk_error_trap_pop ();
        }

        show_timestamps_dialog (manager, "Startup");
        apply_stored_configuration_at_startup (manager, GDK_CURRENT_TIME); /* we don't have a real timestamp at startup anyway */

        gdk_window_add_filter (gdk_get_default_root_window(),
                               (GdkFilterFunc)event_filter,
                               manager);

        start_or_stop_icon (manager);

        gnome_settings_profile_end (NULL);

        return TRUE;
}