static const char *unset_define(cmd_parms *cmd, void *dummy,
                                const char *name)
{
    int i;
    const char **defines;
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

    defines = (const char **)ap_server_config_defines->elts;
    for (i = 0; i < ap_server_config_defines->nelts; i++) {
        if (strcmp(defines[i], name) == 0) {
            defines[i] = *(const char **)apr_array_pop(ap_server_config_defines);
            break;
        }
    }

    if (server_config_defined_vars) {
        apr_table_unset(server_config_defined_vars, name);
    }

    return NULL;
}