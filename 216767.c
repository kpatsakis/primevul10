multi_client_connect_post(struct multi_context *m,
                          struct multi_instance *mi,
                          const char *dc_file,
                          unsigned int option_permissions_mask,
                          unsigned int *option_types_found)
{
    /* Did script generate a dynamic config file? */
    if (platform_test_file(dc_file))
    {
        options_server_import(&mi->context.options,
                              dc_file,
                              D_IMPORT_ERRORS|M_OPTERR,
                              option_permissions_mask,
                              option_types_found,
                              mi->context.c2.es);

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