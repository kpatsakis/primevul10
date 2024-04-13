static int test_ifsection_section(cmd_parms *cmd, const char *arg)
{
    const char *name = apr_pstrcat(cmd->temp_pool, "<", arg, NULL);
    return ap_exists_directive(cmd->temp_pool, name);
}