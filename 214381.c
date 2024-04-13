AP_CORE_DECLARE(void) ap_add_per_url_conf(server_rec *s, void *url_config)
{
    core_server_config *sconf = ap_get_core_module_config(s->module_config);
    void **new_space = (void **)apr_array_push(sconf->sec_url);

    *new_space = url_config;
}