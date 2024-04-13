multi_client_disconnect_script(struct multi_instance *mi)
{
    if ((mi->context.c2.context_auth == CAS_SUCCEEDED && mi->connection_established_flag)
        || mi->context.c2.context_auth == CAS_PARTIAL)
    {
        multi_client_disconnect_setenv(mi);

        if (plugin_defined(mi->context.plugins, OPENVPN_PLUGIN_CLIENT_DISCONNECT))
        {
            if (plugin_call(mi->context.plugins, OPENVPN_PLUGIN_CLIENT_DISCONNECT, NULL, NULL, mi->context.c2.es) != OPENVPN_PLUGIN_FUNC_SUCCESS)
            {
                msg(M_WARN, "WARNING: client-disconnect plugin call failed");
            }
        }

        if (mi->context.options.client_disconnect_script)
        {
            struct argv argv = argv_new();
            setenv_str(mi->context.c2.es, "script_type", "client-disconnect");
            argv_parse_cmd(&argv, mi->context.options.client_disconnect_script);
            openvpn_run_script(&argv, mi->context.c2.es, 0, "--client-disconnect");
            argv_free(&argv);
        }
#ifdef MANAGEMENT_DEF_AUTH
        if (management)
        {
            management_notify_client_close(management, &mi->context.c2.mda_context, mi->context.c2.es);
        }
#endif

    }
}