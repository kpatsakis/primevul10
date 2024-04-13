AP_DECLARE(void **) ap_get_request_note(request_rec *r, apr_size_t note_num)
{
    core_request_config *req_cfg;

    if (note_num >= num_request_notes) {
        return NULL;
    }

    req_cfg = (core_request_config *)
        ap_get_core_module_config(r->request_config);

    if (!req_cfg) {
        return NULL;
    }

    return &(req_cfg->notes[note_num]);
}