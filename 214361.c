static const char *set_hostname_lookups(cmd_parms *cmd, void *d_,
                                        const char *arg)
{
    core_dir_config *d = d_;

    if (!ap_cstr_casecmp(arg, "on")) {
        d->hostname_lookups = HOSTNAME_LOOKUP_ON;
    }
    else if (!ap_cstr_casecmp(arg, "off")) {
        d->hostname_lookups = HOSTNAME_LOOKUP_OFF;
    }
    else if (!ap_cstr_casecmp(arg, "double")) {
        d->hostname_lookups = HOSTNAME_LOOKUP_DOUBLE;
    }
    else {
        return "parameter must be 'on', 'off', or 'double'";
    }

    return NULL;
}