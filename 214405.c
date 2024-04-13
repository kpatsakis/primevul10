static const char *set_http_method(cmd_parms *cmd, void *conf, const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, GLOBAL_ONLY);
    if (err != NULL)
        return err;
    ap_method_register(cmd->pool, arg);
    return NULL;
}