AP_DECLARE(apr_off_t) ap_get_limit_req_body(const request_rec *r)
{
    core_dir_config *d =
      (core_dir_config *)ap_get_core_module_config(r->per_dir_config);

    if (d->limit_req_body == AP_LIMIT_REQ_BODY_UNSET) {
        return AP_DEFAULT_LIMIT_REQ_BODY;
    }

    return d->limit_req_body;
}