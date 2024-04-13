static void *merge_core_dir_configs(apr_pool_t *a, void *basev, void *newv)
{
    core_dir_config *base = (core_dir_config *)basev;
    core_dir_config *new = (core_dir_config *)newv;
    core_dir_config *conf;

    /* Create this conf by duplicating the base, replacing elements
     * (or creating copies for merging) where new-> values exist.
     */
    conf = (core_dir_config *)apr_pmemdup(a, base, sizeof(core_dir_config));

    conf->d = new->d;
    conf->d_is_fnmatch = new->d_is_fnmatch;
    conf->d_components = new->d_components;
    conf->r = new->r;
    conf->refs = new->refs;
    conf->condition = new->condition;

    if (new->opts & OPT_UNSET) {
        /* there was no explicit setting of new->opts, so we merge
         * preserve the invariant (opts_add & opts_remove) == 0
         */
        conf->opts_add = (conf->opts_add & ~new->opts_remove) | new->opts_add;
        conf->opts_remove = (conf->opts_remove & ~new->opts_add)
                            | new->opts_remove;
        conf->opts = (conf->opts & ~conf->opts_remove) | conf->opts_add;

        /* If Includes was enabled with exec in the base config, but
         * was enabled without exec in the new config, then disable
         * exec in the merged set. */
        if (((base->opts & (OPT_INCLUDES|OPT_INC_WITH_EXEC))
             == (OPT_INCLUDES|OPT_INC_WITH_EXEC))
            && ((new->opts & (OPT_INCLUDES|OPT_INC_WITH_EXEC))
                == OPT_INCLUDES)) {
            conf->opts &= ~OPT_INC_WITH_EXEC;
        }
    }
    else {
        /* otherwise we just copy, because an explicit opts setting
         * overrides all earlier +/- modifiers
         */
        conf->opts = new->opts;
        conf->opts_add = new->opts_add;
        conf->opts_remove = new->opts_remove;
    }

    if (!(new->override & OR_UNSET)) {
        conf->override = new->override;
    }

    if (!(new->override_opts & OPT_UNSET)) {
        conf->override_opts = new->override_opts;
    }

    if (new->override_list != NULL) {
        conf->override_list = new->override_list;
    }

    if (conf->response_code_exprs == NULL) {
        conf->response_code_exprs = new->response_code_exprs;
    }
    else if (new->response_code_exprs != NULL) {
        conf->response_code_exprs = apr_hash_overlay(a,
                new->response_code_exprs, conf->response_code_exprs);
    }
    /* Otherwise we simply use the base->response_code_exprs array
     */

    if (new->hostname_lookups != HOSTNAME_LOOKUP_UNSET) {
        conf->hostname_lookups = new->hostname_lookups;
    }

    if (new->content_md5 != AP_CONTENT_MD5_UNSET) {
        conf->content_md5 = new->content_md5;
    }

    if (new->accept_path_info != AP_ACCEPT_PATHINFO_UNSET) {
        conf->accept_path_info = new->accept_path_info;
    }

    if (new->use_canonical_name != USE_CANONICAL_NAME_UNSET) {
        conf->use_canonical_name = new->use_canonical_name;
    }

    if (new->use_canonical_phys_port != USE_CANONICAL_PHYS_PORT_UNSET) {
        conf->use_canonical_phys_port = new->use_canonical_phys_port;
    }

#ifdef RLIMIT_CPU
    if (new->limit_cpu) {
        conf->limit_cpu = new->limit_cpu;
    }
#endif

#if defined(RLIMIT_DATA) || defined(RLIMIT_VMEM) || defined(RLIMIT_AS)
    if (new->limit_mem) {
        conf->limit_mem = new->limit_mem;
    }
#endif

#ifdef RLIMIT_NPROC
    if (new->limit_nproc) {
        conf->limit_nproc = new->limit_nproc;
    }
#endif

    if (new->limit_req_body != AP_LIMIT_REQ_BODY_UNSET) {
        conf->limit_req_body = new->limit_req_body;
    }

    if (new->limit_xml_body != AP_LIMIT_UNSET)
        conf->limit_xml_body = new->limit_xml_body;

    if (!conf->sec_file) {
        conf->sec_file = new->sec_file;
    }
    else if (new->sec_file) {
        /* If we merge, the merge-result must have its own array
         */
        conf->sec_file = apr_array_append(a, base->sec_file, new->sec_file);
    }
    /* Otherwise we simply use the base->sec_file array
     */

    if (!conf->sec_if) {
        conf->sec_if = new->sec_if;
    }
    else if (new->sec_if) {
        /* If we merge, the merge-result must have its own array
         */
        conf->sec_if = apr_array_append(a, base->sec_if, new->sec_if);
    }
    /* Otherwise we simply use the base->sec_if array
     */

    if (new->server_signature != srv_sig_unset) {
        conf->server_signature = new->server_signature;
    }

    if (new->add_default_charset != ADD_DEFAULT_CHARSET_UNSET) {
        conf->add_default_charset = new->add_default_charset;
        conf->add_default_charset_name = new->add_default_charset_name;
    }

    /* Overriding all negotiation
     */
    if (new->mime_type) {
        conf->mime_type = new->mime_type;
    }

    if (new->handler) {
        conf->handler = new->handler;
    }
    if (new->expr_handler) {
        conf->expr_handler = new->expr_handler;
    }

    if (new->output_filters) {
        conf->output_filters = new->output_filters;
    }

    if (new->input_filters) {
        conf->input_filters = new->input_filters;
    }

    /*
     * Now merge the setting of the FileETag directive.
     */
    if (new->etag_bits == ETAG_UNSET) {
        conf->etag_add =
            (conf->etag_add & (~ new->etag_remove)) | new->etag_add;
        conf->etag_remove =
            (conf->etag_remove & (~ new->etag_add)) | new->etag_remove;
        conf->etag_bits =
            (conf->etag_bits & (~ conf->etag_remove)) | conf->etag_add;
    }
    else {
        conf->etag_bits = new->etag_bits;
        conf->etag_add = new->etag_add;
        conf->etag_remove = new->etag_remove;
    }

    if (conf->etag_bits != ETAG_NONE) {
        conf->etag_bits &= (~ ETAG_NONE);
    }

    if (new->enable_mmap != ENABLE_MMAP_UNSET) {
        conf->enable_mmap = new->enable_mmap;
    }

    if (new->enable_sendfile != ENABLE_SENDFILE_UNSET) {
        conf->enable_sendfile = new->enable_sendfile;
    }

    if (new->allow_encoded_slashes_set) {
        conf->allow_encoded_slashes = new->allow_encoded_slashes;
    }
    if (new->decode_encoded_slashes_set) {
        conf->decode_encoded_slashes = new->decode_encoded_slashes;
    }

    if (new->log) {
        if (!conf->log) {
            conf->log = new->log;
        }
        else {
            conf->log = ap_new_log_config(a, new->log);
            ap_merge_log_config(base->log, conf->log);
        }
    }

    conf->max_ranges = new->max_ranges != AP_MAXRANGES_UNSET ? new->max_ranges : base->max_ranges;
    conf->max_overlaps = new->max_overlaps != AP_MAXRANGES_UNSET ? new->max_overlaps : base->max_overlaps;
    conf->max_reversals = new->max_reversals != AP_MAXRANGES_UNSET ? new->max_reversals : base->max_reversals;

    conf->cgi_pass_auth = new->cgi_pass_auth != AP_CGI_PASS_AUTH_UNSET ? new->cgi_pass_auth : base->cgi_pass_auth;

    if (new->cgi_var_rules) {
        if (!conf->cgi_var_rules) {
            conf->cgi_var_rules = new->cgi_var_rules;
        }
        else {
            conf->cgi_var_rules = apr_hash_overlay(a, new->cgi_var_rules, conf->cgi_var_rules);
        }
    }

    AP_CORE_MERGE_FLAG(qualify_redirect_url, conf, base, new);

    return (void*)conf;
}