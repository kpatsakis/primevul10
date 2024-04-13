static const char *update_loglevel(cmd_parms *cmd, struct ap_logconf *log,
                                   const char *arg)
{
    const char *level_str, *err;
    module *module;
    int level;

    level_str = ap_strrchr_c(arg, ':');

    if (level_str == NULL) {
        err = ap_parse_log_level(arg, &log->level);
        if (err != NULL)
            return err;
        ap_reset_module_loglevels(log, APLOG_NO_MODULE);
        ap_log_error(APLOG_MARK, APLOG_TRACE3, 0, cmd->server,
                     "Setting %s for all modules to %s", cmd->cmd->name, arg);
        return NULL;
    }

    arg = apr_pstrmemdup(cmd->temp_pool, arg, level_str - arg);
    level_str++;
    if (!*level_str) {
        return apr_psprintf(cmd->temp_pool, "Module specifier '%s' must be "
                            "followed by a log level keyword", arg);
    }

    err = ap_parse_log_level(level_str, &level);
    if (err != NULL)
        return apr_psprintf(cmd->temp_pool, "%s:%s: %s", arg, level_str, err);

    if ((module = find_module(cmd->server, arg)) == NULL) {
        char *name = apr_psprintf(cmd->temp_pool, "%s_module", arg);
        ap_log_error(APLOG_MARK, APLOG_TRACE6, 0, cmd->server,
                     "Cannot find module '%s', trying '%s'", arg, name);
        module = find_module(cmd->server, name);
    }

    if (module == NULL) {
        return apr_psprintf(cmd->temp_pool, "Cannot find module %s", arg);
    }

    ap_set_module_loglevel(cmd->pool, log, module->module_index, level);
    ap_log_error(APLOG_MARK, APLOG_TRACE3, 0, cmd->server,
                 "Setting %s for module %s to %s", cmd->cmd->name,
                 module->name, level_str);

    return NULL;
}