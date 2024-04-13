static const char *set_override(cmd_parms *cmd, void *d_, const char *l)
{
    core_dir_config *d = d_;
    char *w;
    char *k, *v;
    const char *err;

    /* Throw a warning if we're in <Location> or <Files> */
    if (ap_check_cmd_context(cmd, NOT_IN_LOCATION | NOT_IN_FILES)) {
        ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server, APLOGNO(00114)
                     "Useless use of AllowOverride in line %d of %s.",
                     cmd->directive->line_num, cmd->directive->filename);
    }
    if ((err = ap_check_cmd_context(cmd, NOT_IN_HTACCESS)) != NULL)
        return err;

    d->override = OR_NONE;
    while (l[0]) {
        w = ap_getword_conf(cmd->temp_pool, &l);

        k = w;
        v = strchr(k, '=');
        if (v) {
                *v++ = '\0';
        }

        if (!ap_cstr_casecmp(w, "Limit")) {
            d->override |= OR_LIMIT;
        }
        else if (!ap_cstr_casecmp(k, "Options")) {
            d->override |= OR_OPTIONS;
            if (v)
                set_allow_opts(cmd, &(d->override_opts), v);
            else
                d->override_opts = OPT_ALL;
        }
        else if (!ap_cstr_casecmp(w, "FileInfo")) {
            d->override |= OR_FILEINFO;
        }
        else if (!ap_cstr_casecmp(w, "AuthConfig")) {
            d->override |= OR_AUTHCFG;
        }
        else if (!ap_cstr_casecmp(w, "Indexes")) {
            d->override |= OR_INDEXES;
        }
        else if (!ap_cstr_casecmp(w, "Nonfatal")) {
            if (!ap_cstr_casecmp(v, "Override")) {
                d->override |= NONFATAL_OVERRIDE;
            }
            else if (!ap_cstr_casecmp(v, "Unknown")) {
                d->override |= NONFATAL_UNKNOWN;
            }
            else if (!ap_cstr_casecmp(v, "All")) {
                d->override |= NONFATAL_ALL;
            }
        }
        else if (!ap_cstr_casecmp(w, "None")) {
            d->override = OR_NONE;
        }
        else if (!ap_cstr_casecmp(w, "All")) {
            d->override = OR_ALL;
        }
        else {
            return apr_pstrcat(cmd->pool, "Illegal override option ", w, NULL);
        }

        d->override &= ~OR_UNSET;
    }

    return NULL;
}