static const char *set_max_reversals(cmd_parms *cmd, void *conf_, const char *arg)
{
    core_dir_config *conf = conf_;
    int val = 0;

    if (!ap_cstr_casecmp(arg, "none")) {
        val = AP_MAXRANGES_NORANGES;
    }
    else if (!ap_cstr_casecmp(arg, "default")) {
        val = AP_MAXRANGES_DEFAULT;
    }
    else if (!ap_cstr_casecmp(arg, "unlimited")) {
        val = AP_MAXRANGES_UNLIMITED;
    }
    else {
        val = atoi(arg);
        if (val <= 0)
            return "MaxRangeReversals requires 'none', 'default', 'unlimited' or "
            "a positive integer";
    }

    conf->max_reversals = val;

    return NULL;
}