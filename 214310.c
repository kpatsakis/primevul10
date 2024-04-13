static const char *set_sethandler(cmd_parms *cmd,
                                     void *d_,
                                     const char *arg_)
{
    core_dir_config *dirconf = d_;
    const char *err;
    dirconf->expr_handler = ap_expr_parse_cmd(cmd, arg_,
                                          AP_EXPR_FLAG_STRING_RESULT,
                                          &err, NULL);
    if (err) {
        return apr_pstrcat(cmd->pool,
                "Can't parse expression : ", err, NULL);
    }
    return NULL;
}