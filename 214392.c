static const char *set_content_md5(cmd_parms *cmd, void *d_, int arg)
{
    core_dir_config *d = d_;

    d->content_md5 = arg ? AP_CONTENT_MD5_ON : AP_CONTENT_MD5_OFF;
    return NULL;
}