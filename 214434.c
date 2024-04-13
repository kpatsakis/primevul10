static void set_banner(apr_pool_t *pconf)
{
    if (ap_server_tokens == SrvTk_PRODUCT_ONLY) {
        ap_add_version_component(pconf, AP_SERVER_BASEPRODUCT);
    }
    else if (ap_server_tokens == SrvTk_MINIMAL) {
        ap_add_version_component(pconf, AP_SERVER_BASEVERSION);
    }
    else if (ap_server_tokens == SrvTk_MINOR) {
        ap_add_version_component(pconf, AP_SERVER_BASEPRODUCT "/" AP_SERVER_MINORREVISION);
    }
    else if (ap_server_tokens == SrvTk_MAJOR) {
        ap_add_version_component(pconf, AP_SERVER_BASEPRODUCT "/" AP_SERVER_MAJORVERSION);
    }
    else {
        ap_add_version_component(pconf, AP_SERVER_BASEVERSION " (" PLATFORM ")");
    }

    /*
     * Lock the server_banner string if we're not displaying
     * the full set of tokens
     */
    if (ap_server_tokens != SrvTk_FULL) {
        banner_locked++;
    }
    server_description = AP_SERVER_BASEVERSION " (" PLATFORM ")";
}