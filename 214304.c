static const char *set_protocol(cmd_parms *cmd, void *dummy,
                                const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_LOC_FILE);
    core_server_config *conf =
        ap_get_core_module_config(cmd->server->module_config);
    char* proto;

    if (err != NULL) {
        return err;
    }

    proto = apr_pstrdup(cmd->pool, arg);
    ap_str_tolower(proto);
    conf->protocol = proto;

    return NULL;
}