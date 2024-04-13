static const char *set_loglevel_override(cmd_parms *cmd, void *d_, int argc,
                                         char *const argv[])
{
    core_server_config *sconf;
    conn_log_config *entry;
    int ret, i;
    const char *addr, *mask, *err;

    if (argc < 2)
        return "LogLevelOverride requires at least two arguments";

    entry = apr_pcalloc(cmd->pool, sizeof(conn_log_config));
    sconf = ap_get_core_module_config(cmd->server->module_config);
    if (!sconf->conn_log_level)
        sconf->conn_log_level = apr_array_make(cmd->pool, 4, sizeof(entry));
    APR_ARRAY_PUSH(sconf->conn_log_level, conn_log_config *) = entry;

    addr = argv[0];
    mask = ap_strchr_c(addr, '/');
    if (mask) {
        addr = apr_pstrmemdup(cmd->temp_pool, addr, mask - addr);
        mask++;
    }
    ret = apr_ipsubnet_create(&entry->subnet, addr, mask, cmd->pool);
    if (ret != APR_SUCCESS)
        return "parsing of subnet/netmask failed";

    for (i = 1; i < argc; i++) {
        if ((err = update_loglevel(cmd, &entry->log, argv[i])) != NULL)
            return err;
    }
    return NULL;
}