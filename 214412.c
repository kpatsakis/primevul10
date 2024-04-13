static void register_hooks(apr_pool_t *p)
{
    errorlog_hash = apr_hash_make(p);
    ap_register_log_hooks(p);
    ap_register_config_hooks(p);
    ap_expr_init(p);

    /* create_connection and pre_connection should always be hooked
     * APR_HOOK_REALLY_LAST by core to give other modules the opportunity
     * to install alternate network transports and stop other functions
     * from being run.
     */
    ap_hook_create_connection(core_create_conn, NULL, NULL,
                              APR_HOOK_REALLY_LAST);
    ap_hook_pre_connection(core_pre_connection, NULL, NULL,
                           APR_HOOK_REALLY_LAST);

    ap_hook_pre_config(core_pre_config, NULL, NULL, APR_HOOK_REALLY_FIRST);
    ap_hook_post_config(core_post_config,NULL,NULL,APR_HOOK_REALLY_FIRST);
    ap_hook_check_config(core_check_config,NULL,NULL,APR_HOOK_FIRST);
    ap_hook_test_config(core_dump_config,NULL,NULL,APR_HOOK_FIRST);
    ap_hook_translate_name(ap_core_translate,NULL,NULL,APR_HOOK_REALLY_LAST);
    ap_hook_map_to_storage(core_upgrade_storage,NULL,NULL,APR_HOOK_REALLY_FIRST);
    ap_hook_map_to_storage(core_map_to_storage,NULL,NULL,APR_HOOK_REALLY_LAST);
    ap_hook_open_logs(ap_open_logs,NULL,NULL,APR_HOOK_REALLY_FIRST);
    ap_hook_child_init(core_child_init,NULL,NULL,APR_HOOK_REALLY_FIRST);
    ap_hook_child_init(ap_logs_child_init,NULL,NULL,APR_HOOK_MIDDLE);
    ap_hook_handler(core_upgrade_handler,NULL,NULL,APR_HOOK_REALLY_FIRST);
    ap_hook_handler(default_handler,NULL,NULL,APR_HOOK_REALLY_LAST);
    /* FIXME: I suspect we can eliminate the need for these do_nothings - Ben */
    ap_hook_type_checker(do_nothing,NULL,NULL,APR_HOOK_REALLY_LAST);
    ap_hook_fixups(core_override_type,NULL,NULL,APR_HOOK_REALLY_FIRST);
    ap_hook_create_request(core_create_req, NULL, NULL, APR_HOOK_MIDDLE);
    APR_OPTIONAL_HOOK(proxy, create_req, core_create_proxy_req, NULL, NULL,
                      APR_HOOK_MIDDLE);
    ap_hook_pre_mpm(ap_create_scoreboard, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_child_status(ap_core_child_status, NULL, NULL, APR_HOOK_MIDDLE);
    ap_hook_insert_network_bucket(core_insert_network_bucket, NULL, NULL,
                                  APR_HOOK_REALLY_LAST);
    ap_hook_dirwalk_stat(core_dirwalk_stat, NULL, NULL, APR_HOOK_REALLY_LAST);
    ap_hook_open_htaccess(ap_open_htaccess, NULL, NULL, APR_HOOK_REALLY_LAST);
    ap_hook_optional_fn_retrieve(core_optional_fn_retrieve, NULL, NULL,
                                 APR_HOOK_MIDDLE);
    ap_hook_output_pending(ap_filter_output_pending, NULL, NULL,
            APR_HOOK_MIDDLE);

    /* register the core's insert_filter hook and register core-provided
     * filters
     */
    ap_hook_insert_filter(core_insert_filter, NULL, NULL, APR_HOOK_MIDDLE);

    ap_core_input_filter_handle =
        ap_register_input_filter("CORE_IN", ap_core_input_filter,
                                 NULL, AP_FTYPE_NETWORK);
    ap_content_length_filter_handle =
        ap_register_output_filter("CONTENT_LENGTH", ap_content_length_filter,
                                  NULL, AP_FTYPE_PROTOCOL);
    ap_core_output_filter_handle =
        ap_register_output_filter("CORE", ap_core_output_filter,
                                  NULL, AP_FTYPE_NETWORK);
    ap_request_core_filter_handle =
        ap_register_output_filter("REQ_CORE", ap_request_core_filter,
                                  NULL, AP_FTYPE_TRANSCODE);
    ap_subreq_core_filter_handle =
        ap_register_output_filter("SUBREQ_CORE", ap_sub_req_output_filter,
                                  NULL, AP_FTYPE_CONTENT_SET);
    ap_old_write_func =
        ap_register_output_filter("OLD_WRITE", ap_old_write_filter,
                                  NULL, AP_FTYPE_RESOURCE - 10);
}