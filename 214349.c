static const char *set_protocols_honor_order(cmd_parms *cmd, void *dummy,
                                             const char *arg)
{
    core_server_config *conf =
    ap_get_core_module_config(cmd->server->module_config);
    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_LOC_FILE);
    
    if (err) {
        return err;
    }
    
    if (ap_cstr_casecmp(arg, "on") == 0) {
        conf->protocols_honor_order = 1;
    }
    else if (ap_cstr_casecmp(arg, "off") == 0) {
        conf->protocols_honor_order = 0;
    }
    else {
        return "ProtocolsHonorOrder must be 'on' or 'off'";
    }
    
    return NULL;
}