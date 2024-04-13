static const char *set_merge_trailers(cmd_parms *cmd, void *dummy, int arg)
{
    core_server_config *conf = ap_get_module_config(cmd->server->module_config,
                                                    &core_module);
    conf->merge_trailers = (arg ? AP_MERGE_TRAILERS_ENABLE :
            AP_MERGE_TRAILERS_DISABLE);

    return NULL;
}