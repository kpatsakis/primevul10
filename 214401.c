AP_DECLARE(const char*) ap_get_server_protocol(server_rec* s)
{
    core_server_config *conf = ap_get_core_module_config(s->module_config);
    return conf->protocol;
}