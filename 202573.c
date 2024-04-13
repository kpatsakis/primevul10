refresh_tray_icon_menu_if_active (GsdXrandrManager *manager, guint32 timestamp)
{
        GsdXrandrManagerPrivate *priv = manager->priv;

        if (priv->popup_menu) {
                gtk_menu_shell_cancel (GTK_MENU_SHELL (priv->popup_menu)); /* status_icon_popup_menu_selection_done_cb() will free everything */
                status_icon_popup_menu (manager, 0, timestamp);
        }
}