static const char *set_signature_flag(cmd_parms *cmd, void *d_,
                                      const char *arg)
{
    core_dir_config *d = d_;

    if (ap_cstr_casecmp(arg, "On") == 0) {
        d->server_signature = srv_sig_on;
    }
    else if (ap_cstr_casecmp(arg, "Off") == 0) {
        d->server_signature = srv_sig_off;
    }
    else if (ap_cstr_casecmp(arg, "EMail") == 0) {
        d->server_signature = srv_sig_withmail;
    }
    else {
        return "ServerSignature: use one of: off | on | email";
    }

    return NULL;
}