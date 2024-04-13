static const char *set_serv_tokens(cmd_parms *cmd, void *dummy,
                                   const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);

    if (err != NULL) {
        return err;
    }

    if (!ap_cstr_casecmp(arg, "OS")) {
        ap_server_tokens = SrvTk_OS;
    }
    else if (!ap_cstr_casecmp(arg, "Min") || !ap_cstr_casecmp(arg, "Minimal")) {
        ap_server_tokens = SrvTk_MINIMAL;
    }
    else if (!ap_cstr_casecmp(arg, "Major")) {
        ap_server_tokens = SrvTk_MAJOR;
    }
    else if (!ap_cstr_casecmp(arg, "Minor") ) {
        ap_server_tokens = SrvTk_MINOR;
    }
    else if (!ap_cstr_casecmp(arg, "Prod") || !ap_cstr_casecmp(arg, "ProductOnly")) {
        ap_server_tokens = SrvTk_PRODUCT_ONLY;
    }
    else if (!ap_cstr_casecmp(arg, "Full")) {
        ap_server_tokens = SrvTk_FULL;
    }
    else {
        return "ServerTokens takes 1 argument: 'Prod(uctOnly)', 'Major', 'Minor', 'Min(imal)', 'OS', or 'Full'";
    }

    return NULL;
}