static int core_pre_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp)
{
    ap_mutex_init(pconf);

    if (!saved_server_config_defines)
        init_config_defines(pconf);
    apr_pool_cleanup_register(pconf, NULL, reset_config_defines,
                              apr_pool_cleanup_null);

    mpm_common_pre_config(pconf);

    return OK;
}