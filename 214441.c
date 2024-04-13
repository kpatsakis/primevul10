static const char *set_accept_path_info(cmd_parms *cmd, void *d_, const char *arg)
{
    core_dir_config *d = d_;

    if (ap_cstr_casecmp(arg, "on") == 0) {
        d->accept_path_info = AP_REQ_ACCEPT_PATH_INFO;
    }
    else if (ap_cstr_casecmp(arg, "off") == 0) {
        d->accept_path_info = AP_REQ_REJECT_PATH_INFO;
    }
    else if (ap_cstr_casecmp(arg, "default") == 0) {
        d->accept_path_info = AP_REQ_DEFAULT_PATH_INFO;
    }
    else {
        return "AcceptPathInfo must be set to on, off or default";
    }

    return NULL;
}