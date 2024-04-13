static const char *server_hostname_port(cmd_parms *cmd, void *dummy, const char *arg)
{
    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_LOC_FILE);
    const char *portstr, *part;
    char *scheme;
    int port;

    if (err != NULL) {
        return err;
    }

    if (apr_fnmatch_test(arg))
        return apr_pstrcat(cmd->temp_pool, "Invalid ServerName \"", arg,
                "\" use ServerAlias to set multiple server names.", NULL);

    part = ap_strstr_c(arg, "://");

    if (part) {
      scheme = apr_pstrndup(cmd->pool, arg, part - arg);
      ap_str_tolower(scheme);
      cmd->server->server_scheme = (const char *)scheme;
      part += 3;
    } else {
      part = arg;
    }

    portstr = ap_strchr_c(part, ':');
    if (portstr) {
        cmd->server->server_hostname = apr_pstrndup(cmd->pool, part,
                                                    portstr - part);
        portstr++;
        port = atoi(portstr);
        if (port <= 0 || port >= 65536) { /* 65536 == 1<<16 */
            return apr_pstrcat(cmd->temp_pool, "The port number \"", arg,
                          "\" is outside the appropriate range "
                          "(i.e., 1..65535).", NULL);
        }
    }
    else {
        cmd->server->server_hostname = apr_pstrdup(cmd->pool, part);
        port = 0;
    }

    cmd->server->port = port;
    return NULL;
}