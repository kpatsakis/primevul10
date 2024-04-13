static const char *set_qualify_redirect_url(cmd_parms *cmd, void *d_, int flag)
{
    core_dir_config *d = d_;

    d->qualify_redirect_url = flag ? AP_CORE_CONFIG_ON : AP_CORE_CONFIG_OFF;

    return NULL;
}