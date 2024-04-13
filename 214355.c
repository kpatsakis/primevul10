char *ap_response_code_string(request_rec *r, int error_index)
{
    core_dir_config *dirconf;
    core_request_config *reqconf = ap_get_core_module_config(r->request_config);
    const char *err;
    const char *response;
    ap_expr_info_t *expr;

    /* check for string registered via ap_custom_response() first */
    if (reqconf->response_code_strings != NULL
            && reqconf->response_code_strings[error_index] != NULL) {
        return reqconf->response_code_strings[error_index];
    }

    /* check for string specified via ErrorDocument */
    dirconf = ap_get_core_module_config(r->per_dir_config);

    if (!dirconf->response_code_exprs) {
        return NULL;
    }

    expr = apr_hash_get(dirconf->response_code_exprs, &error_index,
            sizeof(error_index));
    if (!expr) {
        return NULL;
    }

    /* special token to indicate revert back to default */
    if ((char *) expr == &errordocument_default) {
        return NULL;
    }

    err = NULL;
    response = ap_expr_str_exec(r, expr, &err);
    if (err) {
        ap_log_rerror(
                APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(02841) "core: ErrorDocument: can't "
                "evaluate require expression: %s", err);
        return NULL;
    }

    /* alas, duplication required as we return not-const */
    return apr_pstrdup(r->pool, response);
}