AP_CORE_DECLARE(const char *) ap_add_if_conf(apr_pool_t *p,
                                             core_dir_config *conf,
                                             void *if_config)
{
    void **new_space;
    core_dir_config *new = ap_get_module_config(if_config, &core_module);

    if (!conf->sec_if) {
        conf->sec_if = apr_array_make(p, 2, sizeof(ap_conf_vector_t *));
    }
    if (new->condition_ifelse & AP_CONDITION_ELSE) {
        int have_if = 0;
        if (conf->sec_if->nelts > 0) {
            core_dir_config *last;
            ap_conf_vector_t *lastelt = APR_ARRAY_IDX(conf->sec_if,
                                                      conf->sec_if->nelts - 1,
                                                      ap_conf_vector_t *);
            last = ap_get_module_config(lastelt, &core_module);
            if (last->condition_ifelse & AP_CONDITION_IF)
                have_if = 1;
        }
        if (!have_if)
            return "<Else> or <ElseIf> section without previous <If> or "
                   "<ElseIf> section in same scope";
    }

    new_space = (void **)apr_array_push(conf->sec_if);
    *new_space = if_config;
    return NULL;
}