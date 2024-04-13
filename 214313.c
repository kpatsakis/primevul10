static const char *set_protocols(cmd_parms *cmd, void *dummy,
                                 const char *arg)
{
    core_server_config *conf =
    ap_get_core_module_config(cmd->server->module_config);
    const char **np;
    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_LOC_FILE);

    if (err) {
        return err;
    }
    
    np = (const char **)apr_array_push(conf->protocols);
    *np = arg;

    return NULL;
}