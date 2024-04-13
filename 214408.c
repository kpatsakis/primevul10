static void *merge_core_server_configs(apr_pool_t *p, void *basev, void *virtv)
{
    core_server_config *base = (core_server_config *)basev;
    core_server_config *virt = (core_server_config *)virtv;
    core_server_config *conf = (core_server_config *)
                               apr_pmemdup(p, base, sizeof(core_server_config));

    if (virt->ap_document_root)
        conf->ap_document_root = virt->ap_document_root;

    if (virt->access_name)
        conf->access_name = virt->access_name;

    /* XXX optimize to keep base->sec_ pointers if virt->sec_ array is empty */
    conf->sec_dir = apr_array_append(p, base->sec_dir, virt->sec_dir);
    conf->sec_url = apr_array_append(p, base->sec_url, virt->sec_url);

    if (virt->redirect_limit)
        conf->redirect_limit = virt->redirect_limit;

    if (virt->subreq_limit)
        conf->subreq_limit = virt->subreq_limit;

    if (virt->trace_enable != AP_TRACE_UNSET)
        conf->trace_enable = virt->trace_enable;

    if (virt->http09_enable != AP_HTTP09_UNSET)
        conf->http09_enable = virt->http09_enable;

    if (virt->http_conformance != AP_HTTP_CONFORMANCE_UNSET)
        conf->http_conformance = virt->http_conformance;

    if (virt->http_methods != AP_HTTP_METHODS_UNSET)
        conf->http_methods = virt->http_methods;

    if (virt->http_cl_head_zero != AP_HTTP_CL_HEAD_ZERO_UNSET)
        conf->http_cl_head_zero = virt->http_cl_head_zero;

    if (virt->http_expect_strict != AP_HTTP_EXPECT_STRICT_UNSET)
        conf->http_expect_strict = virt->http_expect_strict;

    /* no action for virt->accf_map, not allowed per-vhost */

    if (virt->protocol)
        conf->protocol = virt->protocol;

    if (virt->gprof_dir)
        conf->gprof_dir = virt->gprof_dir;

    if (virt->error_log_format)
        conf->error_log_format = virt->error_log_format;

    if (virt->error_log_conn)
        conf->error_log_conn = virt->error_log_conn;

    if (virt->error_log_req)
        conf->error_log_req = virt->error_log_req;

    if (virt->conn_log_level) {
        if (!conf->conn_log_level) {
            conf->conn_log_level = virt->conn_log_level;
        }
        else {
            /* apr_array_append actually creates a new array */
            conf->conn_log_level = apr_array_append(p, conf->conn_log_level,
                                                    virt->conn_log_level);
        }
    }

    conf->merge_trailers = (virt->merge_trailers != AP_MERGE_TRAILERS_UNSET)
                           ? virt->merge_trailers
                           : base->merge_trailers;

    conf->protocols = ((virt->protocols->nelts > 0) ?
                       virt->protocols : base->protocols);
    conf->protocols_honor_order = ((virt->protocols_honor_order < 0) ?
                                       base->protocols_honor_order :
                                       virt->protocols_honor_order);
    conf->async_filter = ((virt->async_filter_set) ?
                                       virt->async_filter :
                                       base->async_filter);
    conf->async_filter_set = base->async_filter_set || virt->async_filter_set;

    return conf;
}