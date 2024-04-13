AP_CORE_DECLARE(void) ap_add_file_conf(apr_pool_t *p, core_dir_config *conf,
                                       void *url_config)
{
    void **new_space;

    if (!conf->sec_file)
        conf->sec_file = apr_array_make(p, 2, sizeof(ap_conf_vector_t *));

    new_space = (void **)apr_array_push(conf->sec_file);
    *new_space = url_config;
}