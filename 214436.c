AP_DECLARE(const char *) ap_context_document_root(request_rec *r)
{
    core_request_config *conf = ap_get_core_module_config(r->request_config);
    if (conf->context_document_root)
        return conf->context_document_root;
    else
        return ap_document_root(r);
}