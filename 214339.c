AP_CORE_DECLARE(void) ap_init_rng(apr_pool_t *p)
{
    unsigned char seed[8];
    apr_status_t rv;
    rng = apr_random_standard_new(p);
    do {
        rv = apr_generate_random_bytes(seed, sizeof(seed));
        if (rv != APR_SUCCESS)
            goto error;
        apr_random_add_entropy(rng, seed, sizeof(seed));
        rv = apr_random_insecure_ready(rng);
    } while (rv == APR_ENOTENOUGHENTROPY);
    if (rv == APR_SUCCESS)
        return;
error:
    ap_log_error(APLOG_MARK, APLOG_CRIT, rv, NULL, APLOGNO(00141)
                 "Could not initialize random number generator");
    exit(1);
}