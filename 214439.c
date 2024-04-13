static const char *set_cgi_var(cmd_parms *cmd, void *d_,
                               const char *var, const char *rule_)
{
    core_dir_config *d = d_;
    char *rule = apr_pstrdup(cmd->pool, rule_);

    ap_str_tolower(rule);

    if (!strcmp(var, "REQUEST_URI")) {
        if (strcmp(rule, "current-uri") && strcmp(rule, "original-uri")) {
            return "Valid rules for REQUEST_URI are 'current-uri' and 'original-uri'";
        }
    }
    else {
        return apr_pstrcat(cmd->pool, "Unrecognized CGI variable: \"",
                           var, "\"", NULL);
    }

    if (!d->cgi_var_rules) {
        d->cgi_var_rules = apr_hash_make(cmd->pool);
    }
    apr_hash_set(d->cgi_var_rules, var, APR_HASH_KEY_STRING, rule);
    return NULL;
}