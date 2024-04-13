AP_CORE_DECLARE(void) ap_add_per_dir_conf(server_rec *s, void *dir_config)
{
    core_server_config *sconf = ap_get_core_module_config(s->module_config);
    void **new_space = (void **)apr_array_push(sconf->sec_dir);

    *new_space = dir_config;
}