static const char *set_http_protocol_options(cmd_parms *cmd, void *dummy,
                                             const char *arg)
{
    core_server_config *conf =
        ap_get_core_module_config(cmd->server->module_config);

    if (strcasecmp(arg, "allow0.9") == 0)
        conf->http09_enable |= AP_HTTP09_ENABLE;
    else if (strcasecmp(arg, "require1.0") == 0)
        conf->http09_enable |= AP_HTTP09_DISABLE;
    else if (strcasecmp(arg, "strict") == 0)
        conf->http_conformance |= AP_HTTP_CONFORMANCE_STRICT;
    else if (strcasecmp(arg, "unsafe") == 0)
        conf->http_conformance |= AP_HTTP_CONFORMANCE_UNSAFE;
    else if (strcasecmp(arg, "registeredmethods") == 0)
        conf->http_methods |= AP_HTTP_METHODS_REGISTERED;
    else if (strcasecmp(arg, "lenientmethods") == 0)
        conf->http_methods |= AP_HTTP_METHODS_LENIENT;
    else
        return "HttpProtocolOptions accepts "
               "'Unsafe' or 'Strict' (default), "
               "'RegisteredMethods' or 'LenientMethods' (default), and "
               "'Require1.0' or 'Allow0.9' (default)";

    if ((conf->http09_enable & AP_HTTP09_ENABLE)
            && (conf->http09_enable & AP_HTTP09_DISABLE))
        return "HttpProtocolOptions 'Allow0.9' and 'Require1.0'"
               " are mutually exclusive";

    if ((conf->http_conformance & AP_HTTP_CONFORMANCE_STRICT)
            && (conf->http_conformance & AP_HTTP_CONFORMANCE_UNSAFE))
        return "HttpProtocolOptions 'Strict' and 'Unsafe'"
               " are mutually exclusive";

    if ((conf->http_methods & AP_HTTP_METHODS_REGISTERED)
            && (conf->http_methods & AP_HTTP_METHODS_LENIENT))
        return "HttpProtocolOptions 'RegisteredMethods' and 'LenientMethods'"
               " are mutually exclusive";

    return NULL;
}