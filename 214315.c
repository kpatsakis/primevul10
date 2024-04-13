static int reset_config_defines(void *dummy)
{
    ap_server_config_defines = saved_server_config_defines;
    saved_server_config_defines = NULL;
    server_config_defined_vars = NULL;
    return OK;
}