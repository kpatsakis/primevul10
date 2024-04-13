static int core_check_config(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
    int rv = OK;
    while (s) {
        if (check_errorlog_dir(ptemp, s) != OK)
            rv = !OK;
        s = s->next;
    }
    return rv;
}