learn_address_script(const struct multi_context *m,
                     const struct multi_instance *mi,
                     const char *op,
                     const struct mroute_addr *addr)
{
    struct gc_arena gc = gc_new();
    struct env_set *es;
    bool ret = true;
    struct plugin_list *plugins;

    /* get environmental variable source */
    if (mi && mi->context.c2.es)
    {
        es = mi->context.c2.es;
    }
    else
    {
        es = env_set_create(&gc);
    }

    /* get plugin source */
    if (mi)
    {
        plugins = mi->context.plugins;
    }
    else
    {
        plugins = m->top.plugins;
    }

    if (plugin_defined(plugins, OPENVPN_PLUGIN_LEARN_ADDRESS))
    {
        struct argv argv = argv_new();
        argv_printf(&argv, "%s %s",
                    op,
                    mroute_addr_print(addr, &gc));
        if (mi)
        {
            argv_printf_cat(&argv, "%s", tls_common_name(mi->context.c2.tls_multi, false));
        }
        if (plugin_call(plugins, OPENVPN_PLUGIN_LEARN_ADDRESS, &argv, NULL, es) != OPENVPN_PLUGIN_FUNC_SUCCESS)
        {
            msg(M_WARN, "WARNING: learn-address plugin call failed");
            ret = false;
        }
        argv_free(&argv);
    }

    if (m->top.options.learn_address_script)
    {
        struct argv argv = argv_new();
        setenv_str(es, "script_type", "learn-address");
        argv_parse_cmd(&argv, m->top.options.learn_address_script);
        argv_printf_cat(&argv, "%s %s", op, mroute_addr_print(addr, &gc));
        if (mi)
        {
            argv_printf_cat(&argv, "%s", tls_common_name(mi->context.c2.tls_multi, false));
        }
        if (!openvpn_run_script(&argv, es, 0, "--learn-address"))
        {
            ret = false;
        }
        argv_free(&argv);
    }

    gc_free(&gc);
    return ret;
}