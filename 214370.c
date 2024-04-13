static int core_post_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
    ap__logio_add_bytes_out = APR_RETRIEVE_OPTIONAL_FN(ap_logio_add_bytes_out);
    ident_lookup = APR_RETRIEVE_OPTIONAL_FN(ap_ident_lookup);
    ap__authz_ap_some_auth_required = APR_RETRIEVE_OPTIONAL_FN(authz_some_auth_required);
    authn_ap_auth_type = APR_RETRIEVE_OPTIONAL_FN(authn_ap_auth_type);
    authn_ap_auth_name = APR_RETRIEVE_OPTIONAL_FN(authn_ap_auth_name);
    access_compat_ap_satisfies = APR_RETRIEVE_OPTIONAL_FN(access_compat_ap_satisfies);

    set_banner(pconf);
    ap_setup_make_content_type(pconf);
    ap_setup_auth_internal(ptemp);
    if (!sys_privileges) {
        ap_log_error(APLOG_MARK, APLOG_CRIT, 0, NULL, APLOGNO(00136)
                     "Server MUST relinquish startup privileges before "
                     "accepting connections.  Please ensure mod_unixd "
                     "or other system security module is loaded.");
        return !OK;
    }
    apr_pool_cleanup_register(pconf, NULL, ap_mpm_end_gen_helper,
                              apr_pool_cleanup_null);
    return OK;
}