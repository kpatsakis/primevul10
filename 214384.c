static const char *set_override_list(cmd_parms *cmd, void *d_, int argc, char *const argv[])
{
    core_dir_config *d = d_;
    int i;
    const char *err;

    /* Throw a warning if we're in <Location> or <Files> */
    if (ap_check_cmd_context(cmd, NOT_IN_LOCATION | NOT_IN_FILES)) {
        ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server, APLOGNO(00115)
                     "Useless use of AllowOverrideList at %s:%d",
                     cmd->directive->filename, cmd->directive->line_num);
    }
    if ((err = ap_check_cmd_context(cmd, NOT_IN_HTACCESS)) != NULL)
        return err;

    d->override_list = apr_table_make(cmd->pool, argc);

    for (i = 0; i < argc; i++) {
        if (!ap_cstr_casecmp(argv[i], "None")) {
            if (argc != 1) {
                return "'None' not allowed with other directives in "
                       "AllowOverrideList";
            }
            return NULL;
        }
        else {
            const command_rec *result = NULL;
            module *mod = ap_top_module;

            result = ap_find_command_in_modules(argv[i], &mod);
            if (result == NULL) {
                ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server,
                             APLOGNO(00116) "Discarding unrecognized "
                             "directive `%s' in AllowOverrideList at %s:%d",
                             argv[i], cmd->directive->filename,
                             cmd->directive->line_num);
                continue;
            }
            else if ((result->req_override & (OR_ALL|ACCESS_CONF)) == 0) {
                ap_log_error(APLOG_MARK, APLOG_WARNING, 0, cmd->server,
                             APLOGNO(02304) "Discarding directive `%s' not "
                             "allowed in AllowOverrideList at %s:%d",
                             argv[i], cmd->directive->filename,
                             cmd->directive->line_num);
                continue;
            }
            else {
                apr_table_setn(d->override_list, argv[i], "1");
            }
        }
    }

    return NULL;
}