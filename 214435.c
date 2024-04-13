static const char *set_serverpath(cmd_parms *cmd, void *dummy,
                                  const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_LOC_FILE);

    if (err != NULL) {
        return err;
    }

    cmd->server->path = arg;
    cmd->server->pathlen = (int)strlen(arg);
    return NULL;
}