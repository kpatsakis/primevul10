static const char *set_expect_strict(cmd_parms *cmd, void *dummy, int arg)
{
    core_server_config *conf =
        ap_get_core_module_config(cmd->server->module_config);

    if (arg) {
        conf->http_expect_strict = AP_HTTP_EXPECT_STRICT_ENABLE;
    } else {
        conf->http_expect_strict = AP_HTTP_EXPECT_STRICT_DISABLE;
    }
    return NULL;
}