AP_DECLARE(void) ap_add_version_component(apr_pool_t *pconf, const char *component)
{
    if (! banner_locked) {
        /*
         * If the version string is null, register our cleanup to reset the
         * pointer on pool destruction. We also know that, if NULL,
         * we are adding the original SERVER_BASEVERSION string.
         */
        if (server_banner == NULL) {
            apr_pool_cleanup_register(pconf, NULL, reset_banner,
                                      apr_pool_cleanup_null);
            server_banner = apr_pstrdup(pconf, component);
        }
        else {
            /*
             * Tack the given component identifier to the end of
             * the existing string.
             */
            server_banner = apr_pstrcat(pconf, server_banner, " ",
                                        component, NULL);
        }
    }
    server_description = apr_pstrcat(pconf, server_description, " ",
                                     component, NULL);
}