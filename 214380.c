static const char *set_server_string_slot(cmd_parms *cmd, void *dummy,
                                          const char *arg)
{
    /* This one's pretty generic... */

    int offset = (int)(long)cmd->info;
    char *struct_ptr = (char *)cmd->server;

    const char *err = ap_check_cmd_context(cmd,
                                           NOT_IN_DIR_LOC_FILE);
    if (err != NULL) {
        return err;
    }

    *(const char **)(struct_ptr + offset) = arg;
    return NULL;
}