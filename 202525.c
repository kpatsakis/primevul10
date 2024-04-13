add_menu_items_for_output (GsdXrandrManager *manager, GnomeOutputInfo *output)
{
        struct GsdXrandrManagerPrivate *priv = manager->priv;
        GtkWidget *item;

        item = make_menu_item_for_output_title (manager, output);
        gtk_menu_shell_append (GTK_MENU_SHELL (priv->popup_menu), item);

        add_rotation_items_for_output (manager, output);
}