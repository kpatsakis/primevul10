static apr_status_t reset_banner(void *dummy)
{
    banner_locked = 0;
    ap_server_tokens = SrvTk_FULL;
    server_banner = NULL;
    server_description = NULL;
    return APR_SUCCESS;
}