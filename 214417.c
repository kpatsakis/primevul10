static const char *set_loglevel(cmd_parms *cmd, void *config_, const char *arg)
{
    struct ap_logconf *log;

    if (cmd->path) {
        core_dir_config *dconf = config_;
        if (!dconf->log) {
            dconf->log = ap_new_log_config(cmd->pool, NULL);
        }
        log = dconf->log;
    }
    else {
        log = &cmd->server->log;
    }

    if (arg == NULL)
        return "LogLevel requires level keyword or module loglevel specifier";

    return update_loglevel(cmd, log, arg);
}