static const char *set_enable_mmap(cmd_parms *cmd, void *d_,
                                   const char *arg)
{
    core_dir_config *d = d_;

    if (ap_cstr_casecmp(arg, "on") == 0) {
        d->enable_mmap = ENABLE_MMAP_ON;
    }
    else if (ap_cstr_casecmp(arg, "off") == 0) {
        d->enable_mmap = ENABLE_MMAP_OFF;
    }
    else {
        return "parameter must be 'on' or 'off'";
    }

    return NULL;
}