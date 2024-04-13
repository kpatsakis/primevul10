status_icon_popup_menu_selection_done_cb (GtkMenuShell *menu_shell, gpointer data)
{
        GsdXrandrManager *manager = GSD_XRANDR_MANAGER (data);
        struct GsdXrandrManagerPrivate *priv = manager->priv;

        gtk_widget_destroy (priv->popup_menu);
        priv->popup_menu = NULL;

        gnome_rr_labeler_hide (priv->labeler);
        g_object_unref (priv->labeler);
        priv->labeler = NULL;

        gnome_rr_config_free (priv->configuration);
        priv->configuration = NULL;
}