static void *create_core_dir_config(apr_pool_t *a, char *dir)
{
    core_dir_config *conf;

    conf = (core_dir_config *)apr_pcalloc(a, sizeof(core_dir_config));

    /* conf->r and conf->d[_*] are initialized by dirsection() or left NULL */

    conf->opts = dir ? OPT_UNSET : OPT_UNSET|OPT_SYM_LINKS;
    conf->opts_add = conf->opts_remove = OPT_NONE;
    conf->override = OR_UNSET|OR_NONE;
    conf->override_opts = OPT_UNSET | OPT_ALL | OPT_SYM_OWNER | OPT_MULTI;

    conf->content_md5 = AP_CONTENT_MD5_UNSET;
    conf->accept_path_info = AP_ACCEPT_PATHINFO_UNSET;

    conf->use_canonical_name = USE_CANONICAL_NAME_UNSET;
    conf->use_canonical_phys_port = USE_CANONICAL_PHYS_PORT_UNSET;

    conf->hostname_lookups = HOSTNAME_LOOKUP_UNSET;

    /*
     * left as NULL (we use apr_pcalloc):
     * conf->limit_cpu = NULL;
     * conf->limit_mem = NULL;
     * conf->limit_nproc = NULL;
     * conf->sec_file = NULL;
     * conf->sec_if   = NULL;
     */

    conf->limit_req_body = AP_LIMIT_REQ_BODY_UNSET;
    conf->limit_xml_body = AP_LIMIT_UNSET;

    conf->server_signature = srv_sig_unset;

    conf->add_default_charset = ADD_DEFAULT_CHARSET_UNSET;
    conf->add_default_charset_name = DEFAULT_ADD_DEFAULT_CHARSET_NAME;

    /* Overriding all negotiation
     * Set NULL by apr_pcalloc:
     * conf->mime_type = NULL;
     * conf->handler = NULL;
     * conf->output_filters = NULL;
     * conf->input_filters = NULL;
     */

    /*
     * Flag for use of inodes in ETags.
     */
    conf->etag_bits = ETAG_UNSET;
    conf->etag_add = ETAG_UNSET;
    conf->etag_remove = ETAG_UNSET;

    conf->enable_mmap = ENABLE_MMAP_UNSET;
    conf->enable_sendfile = ENABLE_SENDFILE_UNSET;
    conf->allow_encoded_slashes = 0;
    conf->decode_encoded_slashes = 0;

    conf->max_ranges = AP_MAXRANGES_UNSET;
    conf->max_overlaps = AP_MAXRANGES_UNSET;
    conf->max_reversals = AP_MAXRANGES_UNSET;

    conf->cgi_pass_auth = AP_CGI_PASS_AUTH_UNSET;
    conf->qualify_redirect_url = AP_CORE_CONFIG_UNSET; 

    return (void *)conf;
}