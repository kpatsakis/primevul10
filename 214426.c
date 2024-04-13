AP_DECLARE(void) ap_set_context_info(request_rec *r, const char *context_prefix,
                                     const char *context_document_root)
{
    core_request_config *conf = ap_get_core_module_config(r->request_config);
    if (context_prefix)
        conf->context_prefix = context_prefix;
    if (context_document_root)
        conf->context_document_root = context_document_root;
}