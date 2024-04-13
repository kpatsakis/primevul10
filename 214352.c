static int test_iffile_section(cmd_parms *cmd, const char *arg)
{
    const char *relative;
    apr_finfo_t sb;

    relative = ap_server_root_relative(cmd->temp_pool, arg);
    return (apr_stat(&sb, relative, 0, cmd->pool) == APR_SUCCESS);
}