static const char *set_options(cmd_parms *cmd, void *d_, const char *l)
{
    core_dir_config *d = d_;
    allow_options_t opt;
    int first = 1;
    int merge = 0;
    int all_none = 0;
    char action;

    while (l[0]) {
        char *w = ap_getword_conf(cmd->temp_pool, &l);
        action = '\0';

        if (*w == '+' || *w == '-') {
            action = *(w++);
            if (!merge && !first && !all_none) {
                return "Either all Options must start with + or -, or no Option may.";
            }
            merge = 1;
        }
        else if (first) {
            d->opts = OPT_NONE;
        }
        else if (merge) {
            return "Either all Options must start with + or -, or no Option may.";
        }

        if (!ap_cstr_casecmp(w, "Indexes")) {
            opt = OPT_INDEXES;
        }
        else if (!ap_cstr_casecmp(w, "Includes")) {
            opt = (OPT_INCLUDES | OPT_INC_WITH_EXEC);
        }
        else if (!ap_cstr_casecmp(w, "IncludesNOEXEC")) {
            opt = OPT_INCLUDES;
        }
        else if (!ap_cstr_casecmp(w, "FollowSymLinks")) {
            opt = OPT_SYM_LINKS;
        }
        else if (!ap_cstr_casecmp(w, "SymLinksIfOwnerMatch")) {
            opt = OPT_SYM_OWNER;
        }
        else if (!ap_cstr_casecmp(w, "ExecCGI")) {
            opt = OPT_EXECCGI;
        }
        else if (!ap_cstr_casecmp(w, "MultiViews")) {
            opt = OPT_MULTI;
        }
        else if (!ap_cstr_casecmp(w, "RunScripts")) { /* AI backcompat. Yuck */
            opt = OPT_MULTI|OPT_EXECCGI;
        }
        else if (!ap_cstr_casecmp(w, "None")) {
            if (!first) {
                return "'Options None' must be the first Option given.";
            }
            else if (merge) { /* Only works since None may not follow any other option. */
                return "You may not use 'Options +None' or 'Options -None'.";
            }
            opt = OPT_NONE;
            all_none = 1;
        }
        else if (!ap_cstr_casecmp(w, "All")) {
            if (!first) {
                return "'Options All' must be the first option given.";
            }
            else if (merge) { /* Only works since All may not follow any other option. */
                return "You may not use 'Options +All' or 'Options -All'.";
            }
            opt = OPT_ALL;
            all_none = 1;
        }
        else {
            return apr_pstrcat(cmd->pool, "Illegal option ", w, NULL);
        }

        if ( (cmd->override_opts & opt) != opt ) {
            return apr_pstrcat(cmd->pool, "Option ", w, " not allowed here", NULL);
        }
        else if (action == '-') {
            /* we ensure the invariant (d->opts_add & d->opts_remove) == 0 */
            d->opts_remove |= opt;
            d->opts_add &= ~opt;
            d->opts &= ~opt;
        }
        else if (action == '+') {
            d->opts_add |= opt;
            d->opts_remove &= ~opt;
            d->opts |= opt;
        }
        else {
            d->opts |= opt;
        }

        first = 0;
    }

    return NULL;
}