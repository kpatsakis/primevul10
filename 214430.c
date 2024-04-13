static const char *no_set_limit(cmd_parms *cmd, void *conf_,
                                const char *arg, const char *arg2)
{
    ap_log_error(APLOG_MARK, APLOG_ERR, 0, cmd->server, APLOGNO(00118)
                "%s not supported on this platform", cmd->cmd->name);

    return NULL;
}