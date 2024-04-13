register_manager_dbus (GsdXrandrManager *manager)
{
        GError *error = NULL;

        manager->priv->dbus_connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
        if (manager->priv->dbus_connection == NULL) {
                if (error != NULL) {
                        g_warning ("Error getting session bus: %s", error->message);
                        g_error_free (error);
                }
                return FALSE;
        }

        /* Hmm, should we do this in gsd_xrandr_manager_start()? */
        dbus_g_connection_register_g_object (manager->priv->dbus_connection, GSD_XRANDR_DBUS_PATH, G_OBJECT (manager));

        return TRUE;
}