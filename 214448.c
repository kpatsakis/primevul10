static const char *set_allow_opts(cmd_parms *cmd, allow_options_t *opts,
                                  const char *l)
{
    allow_options_t opt;
    int first = 1;

    char *w, *p = (char *) l;
    char *tok_state;

    while ((w = apr_strtok(p, ",", &tok_state)) != NULL) {

        if (first) {
            p = NULL;
            *opts = OPT_NONE;
            first = 0;
        }

        if (!ap_cstr_casecmp(w, "Indexes")) {
            opt = OPT_INDEXES;
        }
        else if (!ap_cstr_casecmp(w, "Includes")) {
            /* If Includes is permitted, both Includes and
             * IncludesNOEXEC may be changed. */
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
            opt = OPT_NONE;
        }
        else if (!ap_cstr_casecmp(w, "All")) {
            opt = OPT_ALL;
        }
        else {
            return apr_pstrcat(cmd->pool, "Illegal option ", w, NULL);
        }

        *opts |= opt;
    }

    (*opts) &= (~OPT_UNSET);

    return NULL;
}