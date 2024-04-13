static const char *set_use_canonical_phys_port(cmd_parms *cmd, void *d_,
                                          const char *arg)
{
    core_dir_config *d = d_;

    if (ap_cstr_casecmp(arg, "on") == 0) {
        d->use_canonical_phys_port = USE_CANONICAL_PHYS_PORT_ON;
    }
    else if (ap_cstr_casecmp(arg, "off") == 0) {
        d->use_canonical_phys_port = USE_CANONICAL_PHYS_PORT_OFF;
    }
    else {
        return "parameter must be 'on' or 'off'";
    }

    return NULL;
}