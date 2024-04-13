AP_DECLARE(void) ap_set_server_protocol(server_rec* s, const char* proto)
{
    core_server_config *conf = ap_get_core_module_config(s->module_config);
    conf->protocol = proto;
}