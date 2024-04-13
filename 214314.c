AP_DECLARE(const char *) ap_context_prefix(request_rec *r)
{
    core_request_config *conf = ap_get_core_module_config(r->request_config);
    if (conf->context_prefix)
        return conf->context_prefix;
    else
        return "";
}