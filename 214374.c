static const char *set_allow2f(cmd_parms *cmd, void *d_, const char *arg)
{
    core_dir_config *d = d_;

    if (0 == ap_cstr_casecmp(arg, "on")) {
        d->allow_encoded_slashes = 1;
        d->decode_encoded_slashes = 1; /* for compatibility with 2.0 & 2.2 */
    } else if (0 == ap_cstr_casecmp(arg, "off")) {
        d->allow_encoded_slashes = 0;
        d->decode_encoded_slashes = 0;
    } else if (0 == ap_cstr_casecmp(arg, "nodecode")) {
        d->allow_encoded_slashes = 1;
        d->decode_encoded_slashes = 0;
    } else {
        return apr_pstrcat(cmd->pool,
                           cmd->cmd->name, " must be On, Off, or NoDecode",
                           NULL);
    }

    d->allow_encoded_slashes_set = 1;
    d->decode_encoded_slashes_set = 1;

    return NULL;
}