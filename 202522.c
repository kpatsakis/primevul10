add_unsupported_rotation_item (GsdXrandrManager *manager)
{
        struct GsdXrandrManagerPrivate *priv = manager->priv;
        GtkWidget *item;
        GtkWidget *label;

        item = gtk_menu_item_new ();

        label = gtk_label_new (NULL);
        gtk_label_set_markup (GTK_LABEL (label), _("<i>Rotation not supported</i>"));
        gtk_container_add (GTK_CONTAINER (item), label);

        gtk_widget_show_all (item);
        gtk_menu_shell_append (GTK_MENU_SHELL (priv->popup_menu), item);
}