AP_DECLARE(void) ap_set_document_root(request_rec *r, const char *document_root)
{
    core_request_config *conf = ap_get_core_module_config(r->request_config);
    conf->document_root = document_root;
}