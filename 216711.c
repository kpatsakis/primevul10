multi_client_connect_setenv(struct multi_context *m,
                            struct multi_instance *mi)
{
    struct gc_arena gc = gc_new();

    /* setenv incoming cert common name for script */
    setenv_str(mi->context.c2.es, "common_name", tls_common_name(mi->context.c2.tls_multi, true));

    /* setenv client real IP address */
    setenv_trusted(mi->context.c2.es, get_link_socket_info(&mi->context));

    /* setenv client virtual IP address */
    multi_set_virtual_addr_env(mi);

    /* setenv connection time */
    {
        const char *created_ascii = time_string(mi->created, 0, false, &gc);
        setenv_str(mi->context.c2.es, "time_ascii", created_ascii);
        setenv_long_long(mi->context.c2.es, "time_unix", mi->created);
    }

    gc_free(&gc);
}