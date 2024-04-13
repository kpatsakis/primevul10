AP_DECLARE(apr_size_t) ap_get_limit_xml_body(const request_rec *r)
{
    core_dir_config *conf;

    conf = ap_get_core_module_config(r->per_dir_config);
    if (conf->limit_xml_body == AP_LIMIT_UNSET)
        return AP_DEFAULT_LIMIT_XML_BODY;

    return (apr_size_t)conf->limit_xml_body;
}