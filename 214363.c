static const char *include_config (cmd_parms *cmd, void *dummy,
                                   const char *name)
{
    ap_directive_t *conftree = NULL;
    const char *conffile, *error;
    unsigned *recursion;
    int optional = cmd->cmd->cmd_data ? 1 : 0;
    void *data;

    /* NOTE: ap_include_sentinel is also used by ap_process_resource_config()
     * during DUMP_INCLUDES; don't change its type or remove it without updating
     * the other.
     */
    apr_pool_userdata_get(&data, "ap_include_sentinel", cmd->pool);
    if (data) {
        recursion = data;
    }
    else {
        data = recursion = apr_palloc(cmd->pool, sizeof(*recursion));
        *recursion = 0;
        apr_pool_userdata_setn(data, "ap_include_sentinel", NULL, cmd->pool);
    }

    if (++*recursion > AP_MAX_INCLUDE_DEPTH) {
        *recursion = 0;
        return apr_psprintf(cmd->pool, "Exceeded maximum include depth of %u, "
                            "There appears to be a recursion.",
                            AP_MAX_INCLUDE_DEPTH);
    }

    conffile = ap_server_root_relative(cmd->pool, name);
    if (!conffile) {
        *recursion = 0;
        return apr_pstrcat(cmd->pool, "Invalid Include path ",
                           name, NULL);
    }

    if (ap_exists_config_define("DUMP_INCLUDES")) {
        unsigned *line_number;

        /* NOTE: ap_include_lineno is used by ap_process_resource_config()
         * during DUMP_INCLUDES; don't change its type or remove it without
         * updating the other.
         */
        apr_pool_userdata_get(&data, "ap_include_lineno", cmd->pool);
        if (data) {
            line_number = data;
        } else {
            data = line_number = apr_palloc(cmd->pool, sizeof(*line_number));
            apr_pool_userdata_setn(data, "ap_include_lineno", NULL, cmd->pool);
        }

        *line_number = cmd->config_file->line_number;
    }

    error = ap_process_fnmatch_configs(cmd->server, conffile, &conftree,
                                       cmd->pool, cmd->temp_pool,
                                       optional);
    if (error) {
        *recursion = 0;
        return error;
    }

    *(ap_directive_t **)dummy = conftree;

    /* recursion level done */
    if (*recursion) {
        --*recursion;
    }

    return NULL;
}