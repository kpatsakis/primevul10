AP_DECLARE(const char *) ap_check_cmd_context(cmd_parms *cmd,
                                              unsigned forbidden)
{
    const char *gt = (cmd->cmd->name[0] == '<'
                      && cmd->cmd->name[strlen(cmd->cmd->name)-1] != '>')
                         ? ">" : "";
    const ap_directive_t *found;

    if ((forbidden & NOT_IN_VIRTUALHOST) && cmd->server->is_virtual) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name, gt,
                           " cannot occur within <VirtualHost> section", NULL);
    }

    if ((forbidden & (NOT_IN_LIMIT | NOT_IN_DIR_LOC_FILE))
        && cmd->limited != -1) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name, gt,
                           " cannot occur within <Limit> or <LimitExcept> "
                           "section", NULL);
    }

    if ((forbidden & NOT_IN_HTACCESS) && (cmd->pool == cmd->temp_pool)) {
         return apr_pstrcat(cmd->pool, cmd->cmd->name, gt,
                            " cannot occur within htaccess files", NULL);
    }

    if ((forbidden & NOT_IN_DIR_LOC_FILE) == NOT_IN_DIR_LOC_FILE) {
        if (cmd->path != NULL) {
            return apr_pstrcat(cmd->pool, cmd->cmd->name, gt,
                            " cannot occur within <Directory/Location/Files> "
                            "section", NULL);
        }
        if (cmd->cmd->req_override & EXEC_ON_READ) {
            /* EXEC_ON_READ must be NOT_IN_DIR_LOC_FILE, if not, it will
             * (deliberately) segfault below in the individual tests...
             */
            return NULL;
        }
    }

    if (((forbidden & NOT_IN_DIRECTORY)
         && ((found = find_parent(cmd->directive, "<Directory"))
             || (found = find_parent(cmd->directive, "<DirectoryMatch"))))
        || ((forbidden & NOT_IN_LOCATION)
            && ((found = find_parent(cmd->directive, "<Location"))
                || (found = find_parent(cmd->directive, "<LocationMatch"))))
        || ((forbidden & NOT_IN_FILES)
            && ((found = find_parent(cmd->directive, "<Files"))
                || (found = find_parent(cmd->directive, "<FilesMatch"))
                || (found = find_parent(cmd->directive, "<If"))
                || (found = find_parent(cmd->directive, "<ElseIf"))
                || (found = find_parent(cmd->directive, "<Else"))))
        || ((forbidden & NOT_IN_PROXY)
            && ((found = find_parent(cmd->directive, "<Proxy"))
                || (found = find_parent(cmd->directive, "<ProxyMatch"))))) {
        return apr_pstrcat(cmd->pool, cmd->cmd->name, gt,
                           " cannot occur within ", found->directive,
                           "> section", NULL);
    }

    return NULL;
}