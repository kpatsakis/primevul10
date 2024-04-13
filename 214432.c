AP_DECLARE(void) ap_register_errorlog_handler(apr_pool_t *p, char *tag,
                                              ap_errorlog_handler_fn_t *handler,
                                              int flags)
{
    ap_errorlog_handler *log_struct = apr_palloc(p, sizeof(*log_struct));
    log_struct->func = handler;
    log_struct->flags = flags;

    apr_hash_set(errorlog_hash, tag, 1, (const void *)log_struct);
}