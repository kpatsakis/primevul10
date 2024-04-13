ensure_current_configuration_is_saved (void)
{
        GnomeRRScreen *rr_screen;
        GnomeRRConfig *rr_config;

        /* Normally, gnome_rr_config_save() creates a backup file based on the
         * old monitors.xml.  However, if *that* file didn't exist, there is
         * nothing from which to create a backup.  So, here we'll save the
         * current/unchanged configuration and then let our caller call
         * gnome_rr_config_save() again with the new/changed configuration, so
         * that there *will* be a backup file in the end.
         */

        rr_screen = gnome_rr_screen_new (gdk_screen_get_default (), NULL, NULL, NULL); /* NULL-GError */
        if (!rr_screen)
                return;

        rr_config = gnome_rr_config_new_current (rr_screen);
        gnome_rr_config_save (rr_config, NULL); /* NULL-GError */

        gnome_rr_config_free (rr_config);
        gnome_rr_screen_destroy (rr_screen);
}