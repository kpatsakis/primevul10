AP_DECLARE(const char *) ap_document_root(request_rec *r) /* Don't use this! */
{
    core_server_config *sconf;
    core_request_config *rconf = ap_get_core_module_config(r->request_config);
    if (rconf->document_root)
        return rconf->document_root;
    sconf = ap_get_core_module_config(r->server->module_config);
    return sconf->ap_document_root;
}