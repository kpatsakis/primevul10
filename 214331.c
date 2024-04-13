AP_DECLARE(void) ap_custom_response(request_rec *r, int status,
                                    const char *string)
{
    core_request_config *conf = ap_get_core_module_config(r->request_config);
    int idx;

    if (conf->response_code_strings == NULL) {
        conf->response_code_strings =
            apr_pcalloc(r->pool,
                        sizeof(*conf->response_code_strings) * RESPONSE_CODES);
    }

    idx = ap_index_of_response(status);

    conf->response_code_strings[idx] =
       ((ap_is_url(string) || (*string == '/')) && (*string != '"')) ?
       apr_pstrdup(r->pool, string) : apr_pstrcat(r->pool, "\"", string, NULL);
}