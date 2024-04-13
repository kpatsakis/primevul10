static const char *set_cgi_pass_auth(cmd_parms *cmd, void *d_, int flag)
{
    core_dir_config *d = d_;

    d->cgi_pass_auth = flag ? AP_CGI_PASS_AUTH_ON : AP_CGI_PASS_AUTH_OFF;

    return NULL;
}