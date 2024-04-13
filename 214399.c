static const char *set_define(cmd_parms *cmd, void *dummy,
                              const char *name, const char *value)
{
    if (cmd->parent && ap_cstr_casecmp(cmd->parent->directive, "<VirtualHost")) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name, " is not valid in ",
                                      cmd->parent->directive, " context", NULL);
    }

    if (ap_strchr_c(name, ':') != NULL) {
        return "Variable name must not contain ':'";
    }

    if (!saved_server_config_defines) {
        init_config_defines(cmd->pool);
    }
    if (!ap_exists_config_define(name)) {
        *(const char **)apr_array_push(ap_server_config_defines) = name;
    }
    if (value) {
        if (!server_config_defined_vars) {
            server_config_defined_vars = apr_table_make(cmd->pool, 5);
        }
        apr_table_setn(server_config_defined_vars, name, value);
    }

    return NULL;
}