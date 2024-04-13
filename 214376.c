static void init_config_defines(apr_pool_t *pconf)
{
    saved_server_config_defines = ap_server_config_defines;
    /* Use apr_array_copy instead of apr_array_copy_hdr because it does not
     * protect from the way unset_define removes entries.
     */
    ap_server_config_defines = apr_array_copy(pconf, ap_server_config_defines);
}