add_menu_items_for_outputs (GsdXrandrManager *manager)
{
        struct GsdXrandrManagerPrivate *priv = manager->priv;
        int i;

        for (i = 0; priv->configuration->outputs[i] != NULL; i++) {
                if (priv->configuration->outputs[i]->connected)
                        add_menu_items_for_output (manager, priv->configuration->outputs[i]);
        }
}