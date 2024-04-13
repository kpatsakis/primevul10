start_or_stop_icon (GsdXrandrManager *manager)
{
        if (gconf_client_get_bool (manager->priv->client, CONF_DIR "/" CONF_KEY, NULL)) {
                status_icon_start (manager);
        }
        else {
                status_icon_stop (manager);
        }
}