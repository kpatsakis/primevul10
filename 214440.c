static const char *set_cl_head_zero(cmd_parms *cmd, void *dummy, int arg)
{
    core_server_config *conf =
        ap_get_core_module_config(cmd->server->module_config);

    if (arg) {
        conf->http_cl_head_zero = AP_HTTP_CL_HEAD_ZERO_ENABLE;
    } else {
        conf->http_cl_head_zero = AP_HTTP_CL_HEAD_ZERO_DISABLE;
    }
    return NULL;
}