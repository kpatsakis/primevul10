multi_client_connect_post_plugin(struct multi_context *m,
                                 struct multi_instance *mi,
                                 const struct plugin_return *pr,
                                 unsigned int option_permissions_mask,
                                 unsigned int *option_types_found)
{
    struct plugin_return config;

    plugin_return_get_column(pr, &config, "config");

    /* Did script generate a dynamic config file? */
    if (plugin_return_defined(&config))
    {
        int i;
        for (i = 0; i < config.n; ++i)
        {
            if (config.list[i] && config.list[i]->value)
            {
                options_string_import(&mi->context.options,
                                      config.list[i]->value,
                                      D_IMPORT_ERRORS|M_OPTERR,
                                      option_permissions_mask,
                                      option_types_found,
                                      mi->context.c2.es);
            }
        }

        /*
         * If the --client-connect script generates a config file
         * with an --ifconfig-push directive, it will override any
         * --ifconfig-push directive from the --client-config-dir
         * directory or any --ifconfig-pool dynamic address.
         */
        multi_select_virtual_addr(m, mi);
        multi_set_virtual_addr_env(mi);
    }
}